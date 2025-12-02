#include "Server.h"
#include "GameUpdate.h"
#include "PlayerOperations.h"
#include "Operations/ServerGameStateUpdater.h"

#include "Networking/EventDefinitions/EventLoginRequest.h"
#include "Networking/EventDefinitions/EventLoginConfirmation.h"
#include "Networking/EventDefinitions/EventLoginDenied.h"
#include "Networking/EventDefinitions/EventDebugMessage.h"
#include "Networking/EventDefinitions/EventSpawnNewPlayer.h"
#include "Networking/EventDefinitions/EventPlayerVelocity.h"
#include "Networking/EventDefinitions/EventPlayerLocation.h"
#include "Networking/EventDefinitions/EventUserInput.h"
#include "Networking/EventDefinitions/EventGamestatePlayerInputHistory.h"
#include "maps/Map_TestAll.h"
#include <algorithm>

const int MAX_PLAYERS = 4;
const int MAX_GAMEOBJECTS = 10000;
//Tickrate in milliseconds per ticks
const int TICKRATE_MS = 10;

void* serverEventHandler(std::unique_ptr<Event> ev, Connection& conn, void* args) {
    struct serverEventHandlerData *handle = (serverEventHandlerData*) args;
    std::lock_guard<std::mutex> queueLockGuard(handle->connectionEventsMutex);
    ServerConnection* serverConn = dynamic_cast<ServerConnection*>(&conn);
    if(serverConn == NULL){
        throw std::runtime_error("did not get a server connection");
    }
    std::tuple<ServerConnection&, std::unique_ptr<Event>> queueEntry(*serverConn, std::move(ev));
    handle->connectionEventsQueue.push(std::move(queueEntry));
    //printf("handling event\n");
    return NULL;
};


Server::Server() : serverSocket(42069){
}

Server::~Server(){

}

void Server::run(){
    //set an example Gamestate for now
    //start a corresponding Socket
    //Profit?
    {
        std::vector<StageObject> stageObjects;
        auto so = new StageObject(0, sf::Vector2f(800.f, 50.f), sf::Vector2f(0.f,550.f));
        stageObjects.push_back(StageObject(0, sf::Vector2f(800.f, 50.f), sf::Vector2f(0.f,550.f)));
        stageObjects.push_back(StageObject(1, sf::Vector2f(300.f, 50.f), sf::Vector2f(0.f,450.f)));
        stageObjects.push_back(StageObject(2, sf::Vector2f(100.f, 50.f), sf::Vector2f(500.f,350.f)));
        stageObjects[0].getShape().setFillColor(sf::Color::Green);
        stageObjects[1].getShape().setFillColor(sf::Color::Green);
        stageObjects[2].getShape().setFillColor(sf::Color::Green);
        std::vector<sf::Vector2f> spawnPoints = {sf::Vector2f(400.f,10.f)};
        Stage s = Stage(1, stageObjects, spawnPoints);
        Stage s2 = createMap_TestAll();
        gameStates[0].setStage(s2);
    }
    serverSocket.setArgs(&eventData);
    serverSocket.setEventHandler(serverEventHandler);
    for(OBJECT_ID_TYPE i = 1; i<= MAX_PLAYERS; i++){
        availablePlayerIds.push(i);
    }
    for(OBJECT_ID_TYPE i = 1 + MAX_PLAYERS; i <= MAX_PLAYERS + MAX_GAMEOBJECTS; i++){
        availableObjectIds.push(i);
    }
    
    mainLoop();
}
void Server::mainLoop(){
    printf("entering main loop\n");
    sf::Clock tickClock;
    while(true){
        currentTick++;
        std::vector<playerInputWithId> playerInputs;
        playerInputs.reserve(numPlayers);
        //copy gameState to next gameState
        gameStates[currentTick % gameStateBufferSize] = gameStates[(currentTick - 1) % gameStateBufferSize];
        
        //debug print all players
/*        std::cout << "GameStateDebugInfo:\n";
        for (Player& p: gameStates[currentTick % gameStateBufferSize].getPlayers()){
            std::cout << "  player present: " << p.getId() << '\n';
        }*/
        
        float deltaTime = tickClock.restart().asSeconds();
        processEvents(playerInputs);
        //apply inputs for players
        for(auto& conn: serverSocket.connections){
            auto pInput = conn->getNextPlayerInput();
            if(pInput.has_value()){
                playerInputWithId input = {.playerId = conn->getPlayerId(), .playerInput = pInput.value()};
                playerInputs.push_back(input);
            }
        }

        //assumeInputs for each Player
        for (Player& player : gameStates[currentTick % gameStateBufferSize].getPlayers()){
            auto it = std::find_if(playerInputs.begin(), playerInputs.end(),
                [player](const playerInputWithId& p){ return p.playerId == player.getId(); });
            if(it == playerInputs.end()){
                if(currentTick == 0){
                    //no inputs yet, just use an empty one
                    std::cout << "first frame, using empty userInput\n";
                    playerInputs.push_back((struct playerInputWithId){.playerId = player.getId()});
                    continue;
                }
                //no input received, copy input from last input
                auto& lastInput = inputHistory[(currentTick-1) % gameStateBufferSize];
                auto itLastInput = std::find_if(lastInput.begin(), lastInput.end(),
                [player](const playerInputWithId& p){ return p.playerId == player.getId(); });
                if(itLastInput != playerInputs.end()){
//                    std::cout << "reusing last userInput with userId: " << itLastInput->playerId <<"\n";
                    playerInputs.push_back(*itLastInput);
//                    playerInputs.push_back(*itLastInput);
//                    std::cout << "emplaced: " << playerInputs.back().playerId << '\n';
                }else{
                    //no last inputs, use empty one
//                    std::cout << "no last inputs, use empty one\n";
                    playerInputs.push_back((struct playerInputWithId){.playerId = player.getId()});
                }
            }else{
//                std::cout << "already have input provided by the user\n";
            }
        }

        //store playerInputs
        this->inputHistory[currentTick % gameStateBufferSize] = playerInputs;
        ServerGameStateUpdater updater(gameStates[currentTick % gameStateBufferSize]);
        updateGame(updater, playerInputs, gameStates[currentTick % gameStateBufferSize], deltaTime);
        someTimesResyncGameState();
        int32_t sleep_ms = TICKRATE_MS - tickClock.getElapsedTime().asMilliseconds();
        if(tickClock.getElapsedTime().asMilliseconds() >= 1){
            std::cout << "Computing tick took " << tickClock.getElapsedTime().asMilliseconds() << "ms\n";
        }
        #if ENABLE_SERVER_RENDERING
        renderer.render(gameStates[currentTick % gameStateBufferSize]);
        renderer.processDisplayEvents();
        #endif
        sf::sleep(sf::milliseconds(TICKRATE_MS) - tickClock.getElapsedTime());
    }
    printf("exiting main loop\n");
}

void Server::processEvents(std::vector<playerInputWithId>& playerInputs){
    std::lock_guard<std::mutex> queueLockGuard(eventData.connectionEventsMutex);
    while(!eventData.connectionEventsQueue.empty()){
//        std::cout<< "processEvents: processing a new event\n";
        auto& connEv = eventData.connectionEventsQueue.front();
        ServerConnection& conn = std::get<0>(connEv);
        Event* ev = std::get<1>(connEv).get();
        //somehow handle tha event
        EventLoginRequest *evLoginRequest = dynamic_cast<EventLoginRequest*>(ev);
        if(evLoginRequest != NULL){
            std::cout << "got a new login request\n";
            if(availablePlayerIds.empty()){
                //no new SLOT
                conn.sendTcpEvent(EventLoginDenied(0));
            }else{
                OBJECT_ID_TYPE nextPlayerId = availablePlayerIds.front();
                availablePlayerIds.pop();
                conn.setPlayerId(nextPlayerId);
                conn.udpRecipientPort = evLoginRequest->udpPort;
                conn.sendTcpEvent(EventLoginConfirmation(nextPlayerId));
                
                //send all players to current player for now, should later be included in a gamestate sync
                for(Player& p : gameStates[currentTick % gameStateBufferSize].getPlayers()){
                    conn.sendTcpEvent(EventSpawnNewPlayer(p.getPosition(), p.getId()));
                }
                gameStates[currentTick % gameStateBufferSize].addPlayer(Player(nextPlayerId, sf::Vector2f(40.f, 40.f), sf::Vector2f(400.f, 10.f)));
                numPlayers++;
                //generate empty inputData for new player
                playerInputs.push_back((playerInputWithId){.playerId = nextPlayerId, .playerInput = (playerInput){.moveLeft = false, .moveRight = false, .jump = false}});

                serverSocket.sendTcpEventToEveryone(EventSpawnNewPlayer(gameStates[currentTick % gameStateBufferSize].getPlayer(nextPlayerId).getPosition(), nextPlayerId));
            }
        }

        EventDebugMessage *evDebug = dynamic_cast<EventDebugMessage*>(ev);
        if(evDebug != NULL){
            std::cout << "got a debug message\n";
            std::cout << "Debug message: " << evDebug->message << '\n';
        }

        EventUserInput *evUserInput = dynamic_cast<EventUserInput*>(ev);
        if(evUserInput != NULL){
//            std::cout << "received user input\n";
            playerInput input = evUserInput->playerInput.playerInput;
            Player& player = gameStates[currentTick % gameStateBufferSize].getPlayer(evUserInput->playerInput.playerId);
            conn.enqueueNextInput(evUserInput->playerInput.playerInput);
        }

//        printf("processEvents: done processing event\n");
        eventData.connectionEventsQueue.pop();
    }
}

void Server::someTimesResyncGameState(){
    #define PLAYERRESYNCTIMER 5
    static unsigned tickCounter = 1;
    tickCounter--;
    if(tickCounter == 0){
        tickCounter = PLAYERRESYNCTIMER;
        resyncGameState();
    }
}

void Server::resyncGameState(){
    // perspective: this functions should send the most recent gameState
    // along with every movement in between the gs and the previous one(and perhaps even one before for safety)
    // so that clients will be able to correctly interfer gameStates between these  
    // that means we have to include all current data and all new inputs since the last 2 synced gameStates
    // std::cout << "resyncing players\n";
    
    static constexpr int SNAPSHOT_DISTANCE = 5;
    static constexpr int TOTAL_INFOS_TO_SEND = SNAPSHOT_DISTANCE * 2;

    EventGamestatePlayerInputHistory syncEvent;
    syncEvent.snapShotDistance = SNAPSHOT_DISTANCE;
    syncEvent.latestAcknowledgedPlayerInput = 0;
    syncEvent.startingGameTick = currentTick - TOTAL_INFOS_TO_SEND + 1;

    for (TICK_TYPE i = syncEvent.startingGameTick; i <= currentTick; i++){
        if((i-syncEvent.startingGameTick)%SNAPSHOT_DISTANCE == 0){
            auto& eventInputs = syncEvent.createCombinedUpdateInfo(gameStates[i % gameStateBufferSize]);
            eventInputs.reserve(inputHistory[i % gameStateBufferSize].size());
            for (auto& pInput : inputHistory[i % gameStateBufferSize])
            {
                eventInputs.emplace_back(pInput);
            }

            std::cout << "server: playerInfosSize: " << syncEvent.updateInfos.back().gsUpdate.playerInfos.size() << '\n';
        }else{
            auto& eventInputs = syncEvent.createNewPlayerInputs();
            eventInputs.reserve(inputHistory[i % gameStateBufferSize].size());
            for (auto& pInput : inputHistory[i % gameStateBufferSize])
            {
                eventInputs.emplace_back(pInput);
            }
        }
    }

    std::cout << "sending " << syncEvent.updateInfos.size() << " updates at starting tick " << syncEvent.startingGameTick << "\n";
    serverSocket.sendUdpEventToEveryone(std::move(syncEvent));
}
