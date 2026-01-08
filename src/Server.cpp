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
#include "Logger.h"
#include "plog/Log.h"
#include <algorithm>

void* serverTcpEventHandler(std::unique_ptr<Event> ev, Connection& conn, void* args) {
    struct serverEventHandlerData *handle = (serverEventHandlerData*) args;
    std::lock_guard<std::mutex> queueLockGuard(handle->connectionEventsMutex);
    ServerConnection* serverConn = dynamic_cast<ServerConnection*>(&conn);
    if(serverConn == NULL){
        throw std::runtime_error("did not get a server connection");
    }
    std::tuple<ServerConnection&, std::unique_ptr<Event>> queueEntry(*serverConn, std::move(ev));
    handle->connectionEventsQueue.push(std::move(queueEntry));
    PLOG_VERBOSE << "handlung tcp event";
    return NULL;
};
void* serverUdpEventHandler(std::unique_ptr<UdpClientSendableEvent> ev, std::optional<sf::IpAddress>& remoteAddress, unsigned short& remotePort, void* args){
    if(!remoteAddress.has_value()){
        PLOG_ERROR << "missing remote address for udp packet";
        return NULL;
    }
    struct serverEventHandlerData *handle = (serverEventHandlerData*) args;
    std::lock_guard<std::mutex> queueLockGuard(handle->connectionEventsMutex);
    ServerConnection* serverConn = nullptr;
    PLOG_DEBUG << "got an udp event";
    for (auto& connPtr : *handle->connections)
    {
        PLOG_VERBOSE << "comparing against connection " << connPtr->udpRecipientIpAdress << ':' << connPtr->udpRecipientPort;
        //if(connPtr->udpRecipientIpAdress == remoteAddress.value() && connPtr->udpRecipientPort == remotePort){
        if(connPtr->getPlayerId() == ev->playerId){
            serverConn = connPtr.get();
        }
    }
    
    if(serverConn == NULL){
        PLOG_ERROR << "did not find receipient for " << remoteAddress.value() << ':' << remotePort;
    }
    std::tuple<ServerConnection&, std::unique_ptr<Event>> queueEntry(*serverConn, std::move(ev));
    handle->connectionEventsQueue.push(std::move(queueEntry));
    return NULL;
}



Server::Server() : serverSocket(42069){
    eventData.connections = &serverSocket.connections;
}

Server::Server(unsigned short port) : serverSocket(port) {
    eventData.connections = &serverSocket.connections;
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
        //just push a few gameStates so clients actually have something to display
        gameStates.push(GameState());
        inputHistory.push(std::vector<indexedPlayerInputWithId>());
        currentTick = 0;
        for(int i = 0; i < 64; i++){
            gameStates.push(GameState());
            currentTick++;
            gameStates[currentTick].setStage(s2);
            
            inputHistory.push(std::vector<indexedPlayerInputWithId>());
        }
    }
    serverSocket.setArgs(&eventData);
    serverSocket.setEventHandler(serverTcpEventHandler);
    serverSocket.setUdpArgs(&eventData);
    serverSocket.setUdpEventHandler(serverUdpEventHandler);
    for(OBJECT_ID_TYPE i = 1; i<= MAX_PLAYERS; i++){
        availablePlayerIds.push(i);
    }
    for(OBJECT_ID_TYPE i = 1 + MAX_PLAYERS; i <= MAX_PLAYERS + MAX_GAMEOBJECTS; i++){
        availableObjectIds.push(i);
    }
    
    ready.store(true);
    mainLoop();
    ready.store(false);
}
void Server::mainLoop(){
    PLOG_INFO << "entering main loop";
    sf::Clock tickClock;
    tickClock.start();
    sf::Time startTime = tickClock.getElapsedTime();
    sf::Time latestElapsedTick = startTime;
    while(true){
        sf::Time startTickTime = tickClock.getElapsedTime();
        sf::Time currentDeltaTime = startTickTime - latestElapsedTick;
        if(currentDeltaTime < tickRate){
            PLOG_VERBOSE << "not there yet";
            sf::sleep(tickRate - currentDeltaTime);
            continue;
        }
        latestElapsedTick += tickRate;
        currentTick++;
        PLOG_VERBOSE << "calculating tick " << currentTick;
        std::vector<indexedPlayerInputWithId> playerInputs;
        playerInputs.reserve(numPlayers);
        //copy gameState to next gameState
        gameStates.back();
        gameStates.push(gameStates[currentTick-1]);
        gameStates[currentTick] = gameStates[currentTick-1];

        processEvents(playerInputs);
        //get inputs for players
        for(auto& conn: serverSocket.connections){
            auto pInput = conn->getNextPlayerInput();
            if(pInput.has_value()){
                indexedPlayerInputWithId input(pInput.value(), conn->getPlayerId());
                playerInputs.push_back(input);
            }else{
                PLOG_VERBOSE << "got no input available";
            }
        }

        //assumeInputs for each Player
        for (Player& player : gameStates[currentTick].getPlayers()){
            auto it = std::find_if(playerInputs.begin(), playerInputs.end(),
                [player](const indexedPlayerInputWithId& p){ return p.playerInputWithId.playerId == player.getId(); });
            if(it == playerInputs.end()){
                if(currentTick == 0){
                    //no inputs yet, just use an empty one
                    PLOG_VERBOSE << "first frame, using empty userInput";
                    playerInputs.emplace_back(0, playerInput(), player.getId());
                    //player.getId(), playerInput()
                    playerInputs.back().invalidateIdx();
                    continue;
                }
                //no input received, copy input from last input
                auto& lastInputs = inputHistory[currentTick-1];
                auto itLastInput = std::find_if(lastInputs.begin(), lastInputs.end(),
                [player](const indexedPlayerInputWithId& p){ return p.playerInputWithId.playerId == player.getId(); });
                if(itLastInput != lastInputs.end()){
                    PLOG_DEBUG << "reusing last userInput with userId: " << itLastInput->playerInputWithId.playerId;
                    playerInputs.push_back(*itLastInput);
                    playerInputs.back().invalidateIdx();
                }else{
                    //no last inputs, use empty one
                    playerInputs.emplace_back(0, playerInput(), player.getId());
                    playerInputs.back().invalidateIdx();
                }
            }else{
//                PLOG_VERBOSE << "already have input provided by the user\n";
            }
        }

        //store playerInputs
        inputHistory.push(playerInputs);
        ServerGameStateUpdater updater(gameStates[currentTick]);
        for(auto& input : playerInputs){
            input.playerInputWithId.applyUpdate(updater, gameStates[currentTick]);
        }
        updateGame(updater, gameStates[currentTick], tickRate.asSeconds());

        // --- ROUND AND GAME WIN LOGIC ---
        auto& players = gameStates[currentTick].getPlayers();
        int aliveCount = 0;
        Player* lastAlive = nullptr;
        for (auto& p : players) {
            if (p.getHealth() > 0) {
                aliveCount++;
                lastAlive = &p;
            }
        }

        if (aliveCount == 1 && lastAlive) {
            lastAlive->setScore(lastAlive->getScore() + 1); // or use a setter
            // Notify clients: lastAlive->getId() won the round
            if (lastAlive->getScore() >= 10) {
                // Notify clients: lastAlive->getId() won the game
                // Optionally reset scores, return to lobby, etc.
            }
            // Reset round: restore health, positions, set readyToPlay = false, etc.
        }       

        someTimesResyncGameState();
        int32_t sleep_ms = TICKRATE_MS - tickClock.getElapsedTime().asMilliseconds();

        #if ENABLE_SERVER_RENDERING
        renderer.render(gameStates[currentTick]);
        renderer.processDisplayEvents();
        renderer.display();
        #endif
        sf::Time tickComputeTime = tickClock.getElapsedTime() - startTickTime;
        if(tickComputeTime.asMilliseconds() >= 1){
            PLOG_INFO << "Computing tick took " << tickClock.getElapsedTime().asMilliseconds() << "ms";
        }
    }
    PLOG_INFO <<"exiting main loop";
}

void Server::processEvents(std::vector<indexedPlayerInputWithId>& playerInputs){
    std::lock_guard<std::mutex> queueLockGuard(eventData.connectionEventsMutex);
    while(!eventData.connectionEventsQueue.empty()){
        auto& connEv = eventData.connectionEventsQueue.front();
        ServerConnection& conn = std::get<0>(connEv);
        Event* ev = std::get<1>(connEv).get();
        //somehow handle tha event
        EventLoginRequest *evLoginRequest = dynamic_cast<EventLoginRequest*>(ev);
        if(evLoginRequest != NULL){
            PLOG_INFO << "got a new login request";
            if(availablePlayerIds.empty()){
                //no new SLOT
                conn.sendTcpEvent(EventLoginDenied(0));
            }else{
                OBJECT_ID_TYPE nextPlayerId = availablePlayerIds.front();
                availablePlayerIds.pop();
                conn.setPlayerId(nextPlayerId);
                conn.udpRecipientPort = evLoginRequest->udpPort;
                conn.sendTcpEvent(EventLoginConfirmation(nextPlayerId, currentTick, TICKRATE_MS));
                
                //send all players to current player for now, should later be included in a gamestate sync
                for(Player& p : gameStates[currentTick].getPlayers()){
                    conn.sendTcpEvent(EventSpawnNewPlayer(p.getPosition(), p.getId()));
                }
                gameStates[currentTick].addPlayer(Player(nextPlayerId, sf::Vector2f(40.f, 40.f), sf::Vector2f(400.f, 10.f)));
                numPlayers++;
                //generate empty inputData for new player
                playerInputs.emplace_back(0, playerInput(), nextPlayerId);

                serverSocket.sendTcpEventToEveryone(EventSpawnNewPlayer(gameStates[currentTick].getPlayer(nextPlayerId).getPosition(), nextPlayerId));
            }
        }

        EventDebugMessage *evDebug = dynamic_cast<EventDebugMessage*>(ev);
        if(evDebug != NULL){
            PLOG_VERBOSE << "got a debug message";
            PLOG_DEBUG << "Debug message: " << evDebug->message;
        }

        EventUserInput *evUserInput = dynamic_cast<EventUserInput*>(ev);
        if(evUserInput != NULL){
            PLOG_DEBUG << "received user input";
            while (evUserInput->hasNextUserInput())
            {
                auto input = evUserInput->getNextUserInput();
                PLOG_DEBUG << "adding user input '" << input.idx << "' to queue";
                conn.enqueueInput(input);
            }
        }

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
    
    static constexpr int SNAPSHOT_DISTANCE = 5;
    static constexpr int MAX_TOTAL_INFOS_TO_SEND = SNAPSHOT_DISTANCE * 2;

    int infosToSend = MAX_TOTAL_INFOS_TO_SEND;
    //current Tick = 1 == can send 2 ticks
    if(currentTick + 1 < MAX_TOTAL_INFOS_TO_SEND){
        infosToSend = currentTick + 1;
    }
    
    for (std::unique_ptr<ServerConnection> &connPtr : serverSocket.connections)
    {
        EventGamestatePlayerInputHistory syncEvent = EventGamestatePlayerInputHistory();
        syncEvent.snapShotDistance = SNAPSHOT_DISTANCE;
        syncEvent.startingGameTick = currentTick + 1 - infosToSend;

    //    for (TICK_TYPE i = syncEvent.startingGameTick; i <= currentTick; i++){
        for (TICK_TYPE i = 0; i < infosToSend; i++){
            TICK_TYPE currentTickToSend = syncEvent.startingGameTick + i;
            PLOG_DEBUG << "tick(current, currentToSend, i): " << currentTick << ' ' << currentTickToSend << ' ' << i;
            if((i)%SNAPSHOT_DISTANCE == 0){
                auto& eventUpdates = syncEvent.createCombinedUpdateInfo(gameStates[currentTickToSend], 0-1);
                eventUpdates.pInput.reserve(inputHistory[currentTickToSend].size());
                for (auto& pInput : inputHistory[currentTickToSend])
                {
                    if(pInput.playerInputWithId.playerId == connPtr->getPlayerId()){
                        eventUpdates.gsUpdate.latestIncludedPInput = pInput.idx;
                    }
                    eventUpdates.pInput.emplace_back(pInput.playerInputWithId);
                }

                PLOG_VERBOSE << "server: playerInfosSize: " << syncEvent.updateInfos.back().gsUpdate.playerInfos.size();
            }else{
                auto& eventInputs = syncEvent.createNewPlayerInputs();
                eventInputs.reserve(inputHistory[currentTickToSend].size());
                for (auto& pInput : inputHistory[currentTickToSend])
                {
                    eventInputs.emplace_back(pInput.playerInputWithId);
                }
            }
        }
        
        //set latestUpdatedInputSync
        connPtr->sendUdpEvent(syncEvent);
        /* code */
        PLOG_DEBUG << "sending " << syncEvent.updateInfos.size() << " updates at starting tick " << syncEvent.startingGameTick;
    }
    
    
//    serverSocket.sendUdpEventToEveryone(std::move(syncEvent));
}
