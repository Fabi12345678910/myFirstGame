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
#include "maps/Map_TestAll.h"


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
        currentFrame++;
        std::vector<playerInputWithId> playerInputs;
        playerInputs.reserve(numPlayers);
        //copy gameState to next gameState
        gameStates[currentFrame % gameStateBufferSize] = gameStates[(currentFrame - 1) % gameStateBufferSize];
        float deltaTime = tickClock.restart().asSeconds();
        processEvents(playerInputs);
        //assumeInputs for each Player
        ServerGameStateUpdater updater(gameStates[currentFrame % gameStateBufferSize]);
        updateGame(updater, playerInputs, gameStates[currentFrame % gameStateBufferSize], deltaTime);
        someTimesResyncGameState();
        int32_t sleep_ms = TICKRATE_MS - tickClock.getElapsedTime().asMilliseconds();
        if(tickClock.getElapsedTime().asMilliseconds() >= 1){
            std::cout << "Computing tick took " << tickClock.getElapsedTime().asMilliseconds() << "ms\n";
        }
        #if ENABLE_SERVER_RENDERING
        renderer.render(gameStates[currentFrame % gameStateBufferSize]);
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
                for(Player& p : gameStates[currentFrame % gameStateBufferSize].getPlayers()){
                    conn.sendTcpEvent(EventSpawnNewPlayer(p.getPosition(), p.getId()));
                }
                gameStates[currentFrame % gameStateBufferSize].addPlayer(Player(nextPlayerId, sf::Vector2f(40.f, 40.f), sf::Vector2f(400.f, 10.f)));
                numPlayers++;
                //generate empty inputData for new player
                playerInputs.push_back((playerInputWithId){.playerId = nextPlayerId, .playerInput = (playerInput){.moveLeft = false, .moveRight = false, .jump = false}});

                serverSocket.sendEventToEveryone(EventSpawnNewPlayer(gameStates[currentFrame % gameStateBufferSize].getPlayer(nextPlayerId).getPosition(), nextPlayerId));
            }
        }

        // EventPlayerVelocity *evVelocity = dynamic_cast<EventPlayerVelocity*>(ev);
        // if(evVelocity != NULL){
        //     std::cout << "received velocity update: " << evVelocity->velocity.x << ',' << evVelocity->velocity.y << '\n';
        //     updatePlayerVelocity(gameState, conn.getPlayerId(), evVelocity->velocity);
        //     serverSocket.sendEventToEveryone(EventPlayerVelocity(evVelocity->playerId, evVelocity->velocity));
        // }

        EventDebugMessage *evDebug = dynamic_cast<EventDebugMessage*>(ev);
        if(evDebug != NULL){
            std::cout << "got a debug message\n";
            std::cout << "Debug message: " << evDebug->message << '\n';
        }

        EventUserInput *evUserInput = dynamic_cast<EventUserInput*>(ev);
        if(evUserInput != NULL){
            std::cout << "received user input\n";
            playerInput input = evUserInput->playerInput.playerInput;
            Player& player = gameState.getPlayer(evUserInput->playerInput.playerId);
            
            // Horizontal velocity
            sf::Vector2f v = player.getVelocity();
            v.x = 0.f;
            if(input.moveLeft) { player.setFacing(-1); v.x -= player.getSpeed(); }
            if(input.moveRight) { player.setFacing( 1); v.x += player.getSpeed(); }

            // Jump
            if(input.jump && player.getIsOnGround()){
                v.y = -400.f;
                player.setIsOnGround(false);
            }

            // apply velocity
            player.setVelocity(v);

            // fire projectile
            if(input.projectile && player.getCooldown() == 0){
                int projId = gameState.getBulletIDs();
                gameState.setBulletIDs(projId + 1);

                Projectile proj(projId, {20.f,20.f}, player.getPosition());
                proj.setSpeed(proj.getSpeed() * player.getFacing());
                gameState.addProjectile(proj);

                player.setCooldown(100);
            }
            serverSocket.sendEventToEveryone(EventUserInput(evUserInput->playerInput));
        }

        printf("processEvents: done processing event\n");
        eventData.connectionEventsQueue.pop();
    }
}

void Server::someTimesResyncGameState(){
    #define PLAYERRESYNCTIMER 1
    static unsigned tickCounter = PLAYERRESYNCTIMER;
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
    for(Player &p : gameStates[currentFrame % gameStateBufferSize].getPlayers()){
        serverSocket.sendEventToEveryone(EventPlayerLocation(p.getId(), p.getPosition()));
    }
}
