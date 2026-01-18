#include "Server.h"
#include "GameState.h"
#include "GameUpdate.h"
#include "Config.h"
#include "Operations/ServerGameStateUpdater.h"
#include "StageManager.h"

#include "Networking/EventDefinitions/EventLoginRequest.h"
#include "Networking/EventDefinitions/EventLoginConfirmation.h"
#include "Networking/EventDefinitions/EventLoginDenied.h"
#include "Networking/EventDefinitions/EventDebugMessage.h"
#include "Networking/EventDefinitions/EventSpawnNewPlayer.h"
#include "Networking/EventDefinitions/EventUserInput.h"
#include "Networking/EventDefinitions/EventGamestatePlayerInputHistory.h"
#include "Networking/EventDefinitions/EventSelectMap.h"
#include "Networking/EventDefinitions/EventSelectedMap.h"
#include "Networking/EventDefinitions/EventStartGame.h"

#include "Networking/EventDefinitions/EventServerHealth.h"
#include "Logger.h"
#include "plog/Log.h"
#include <SFML/System/Time.hpp>
#include <algorithm>
#include <random>
#include <stdexcept>

void* serverTcpEventHandler(std::unique_ptr<Event> ev, Connection& conn, void* args) {
    struct serverEventHandlerData *handle = (serverEventHandlerData*) args;
    std::lock_guard<std::mutex> queueLockGuard(handle->connectionEventsMutex);
    ServerConnection* serverConn = dynamic_cast<ServerConnection*>(&conn);
    if(serverConn == NULL){
        PLOG_ERROR << "did not get a server connection";
        return NULL;
    }
    std::tuple<ServerConnection&, std::unique_ptr<Event>> queueEntry(*serverConn, std::move(ev));
    handle->connectionEventsQueue.push(std::move(queueEntry));
    PLOG_VERBOSE_IF(debugServerNetworking) << "handlung tcp event";
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
    PLOG_DEBUG_IF(debugServerNetworking) << "got an udp event";
    for (auto& connPtr : *handle->connections)
    {
        PLOG_VERBOSE_IF(debugServerNetworking) << "comparing against connection " << connPtr->udpRecipientIpAdress << ':' << connPtr->udpRecipientPort;
        //if(connPtr->udpRecipientIpAdress == remoteAddress.value() && connPtr->udpRecipientPort == remotePort){
        if(connPtr->getPlayerId() == ev->playerId){
            if(connPtr->udpRecipientPort != remotePort){
                connPtr->udpRecipientPort = remotePort;
            }
            if(connPtr->udpRecipientIpAdress != remoteAddress.value()){
                connPtr->udpRecipientIpAdress = remoteAddress.value();
            }
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
    initAlwaysOnLogger();
    PLOG_INFO_(1) << "starting server";
    {
        Stage lobbyStage = StageManager::loadStage(1);
        //just push a few gameStates so clients actually have something to display
        gameStates.push(GameState());
        inputHistory.push(std::vector<indexedPlayerInputWithId>());
        currentTick = 0;
        for(int i = 0; i < 64; i++){
            gameStates.push(GameState());
            currentTick++;
            gameStates[currentTick].setStage(lobbyStage);
            
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
    TICK_TYPE selectUntil;

    while(true){
        sf::Time startTickTime = tickClock.getElapsedTime();
        sf::Time currentDeltaTime = startTickTime - latestElapsedTick;
        if(currentDeltaTime < tickRate){
            PLOG_VERBOSE_IF(debugServerGenerations) << "not there yet";
            sf::sleep(tickRate - currentDeltaTime);
            continue;
        }
        latestElapsedTick += tickRate;
        currentTick++;
        PLOG_VERBOSE_IF(debugServerGenerations) << "calculating tick " << currentTick;
        std::vector<indexedPlayerInputWithId> playerInputs;
        playerInputs.reserve(numPlayers);
        //copy gameState to next gameState
        gameStates.back();
        gameStates.push(gameStates[currentTick-1]);
        gameStates[currentTick] = gameStates[currentTick-1];

        if (gameStates[currentTick].getGameState() == gameState::STARTING &&
            currentTick >= gameStates[currentTick].getGameStartTick()) {
            gameStates[currentTick].setGameState(gameState::RUNNING);
        }

        processEvents(playerInputs);
        //get inputs for players
        for(auto& conn: serverSocket.connections){
            auto pInput = conn->getNextPlayerInput();
            if(pInput.has_value()){
                indexedPlayerInputWithId input(pInput.value(), conn->getPlayerId());
                playerInputs.push_back(input);
            }else{
                PLOG_VERBOSE_IF(debugServerInputProcessing) << "got no input available";
            }
        }

        //assumeInputs for each Player
        for (Player& player : gameStates[currentTick].getPlayers()){
            auto it = std::find_if(playerInputs.begin(), playerInputs.end(),
                [player](const indexedPlayerInputWithId& p){ return p.playerInputWithId.playerId == player.getId(); });
            if(it == playerInputs.end()){
                if(currentTick == 0){
                    //no inputs yet, just use an empty one
                    PLOG_VERBOSE_IF(debugServerInputProcessing) << "first frame, using empty userInput";
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
                    PLOG_DEBUG_IF(debugServerInputProcessing) << "reusing last userInput with userId: " << itLastInput->playerInputWithId.playerId;
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

        PLOG_DEBUG_IF(debugServerGameState) << "generated gamestate: " << GameState::toString(gameStates[currentTick].getGameState());
        {
            auto& gs = gameStates[currentTick];
            const auto state = gs.getGameState();
            const auto& spawns = gs.getStage().getSpawnPoints();

            // Outside RUNNING, nobody stays dead.
            if (state != gameState::RUNNING) {
                respawnAtTick.clear();
                for (auto& p : gs.getPlayers()) {
                    if (p.getHealth() <= 0) {
                        p.setHealth(10.f);
                        p.setVelocity({0.f, 0.f});
                        p.setProjectileCooldown(0);
                        if (!spawns.empty()) {
                            const std::size_t spawnIdx = static_cast<std::size_t>((p.getId() - 1) % spawns.size());
                            p.setPosition(spawns[spawnIdx]);
                        }
                    }
                }
            }
            // During RUNNING, players can die and stay dead until the round ends.
            else {
                respawnAtTick.clear();
            }
        }

        // --- ROUND AND GAME WIN LOGIC ---

        // --- Start ---

        if(gameStates[currentTick].getGameState() == gameState::LOBBY){
            if(gameStates[currentTick].getPlayers().size() >= 2){
                bool allReady = true; 
                for (const auto& p : gameStates[currentTick].getPlayers()) {
                    if (!p.getReadyToPlay()) {
                        allReady = false;
                        break;
                    }
                }
                if (allReady) {
                    selectUntil = currentTick + sf::seconds(mapSelectionTimeS)/tickRate;
                    serverSocket.sendTcpEventToEveryone(EventSelectMap(selectUntil));
                    PLOG_INFO_IF(debugServerNetworking) << "sending [Event] Players Ready";

                    selectedMaps.byPlayer.clear();
                    selectedMaps.arrivalOrder.clear();
                    gameStates[currentTick].setGameState(gameState::MAP_SELECT);
                }
            }
        }
        if (gameStates[currentTick].getGameState() == gameState::MAP_SELECT) {
            if (currentTick >= selectUntil + 100 ) {

                static std::mt19937 gen(std::random_device{}());

                std::vector<std::pair<int16_t, std::string>> stageList = StageManager::loadStageList();
                int16_t chosenStageId;

                if (selectedMaps.byPlayer.empty()) {
                    std::uniform_int_distribution<size_t> dist(0, stageList.size() - 1);
                    chosenStageId = stageList[dist(gen)].first;
                }
                else {
                    std::unordered_map<int16_t, int> voteCount;
                    voteCount.reserve(selectedMaps.byPlayer.size());
                    for (const auto& kv : selectedMaps.byPlayer) {
                        voteCount[kv.second]++;
                    }
                    int bestVotes = -1;
                    bool bestSet = false;
                    for (OBJECT_ID_TYPE pid : selectedMaps.arrivalOrder) {
                        auto it = selectedMaps.byPlayer.find(pid);
                        if (it == selectedMaps.byPlayer.end()) continue;
                        const int16_t stageId = it->second;
                        const int votes = voteCount[stageId];
                        if (!bestSet || votes > bestVotes) {
                            bestSet = true;
                            bestVotes = votes;
                            chosenStageId = stageId;
                        }
                    }
                    if (!bestSet) {
                        // Fallback: deterministic smallest stageId.
                        chosenStageId = selectedMaps.byPlayer.begin()->second;
                        for (const auto& kv : selectedMaps.byPlayer) {
                            chosenStageId = std::min(chosenStageId, kv.second);
                        }
                    }
                }

                Stage selectedStage = StageManager::loadStage(chosenStageId);
                const auto& spawns = selectedStage.getSpawnPoints();
                if (spawns.empty()) {
                    PLOG_ERROR << "Selected stage has no spawn points";
                    return;
                }

                // Assign spawns deterministically by sorting players by id.
                std::vector<Player*> playersSorted;
                playersSorted.reserve(gameStates[currentTick].getPlayers().size());
                for (auto& p : gameStates[currentTick].getPlayers()) {
                    playersSorted.push_back(&p);
                }
                std::sort(playersSorted.begin(), playersSorted.end(), [](const Player* a, const Player* b) {
                    return a->getId() < b->getId();
                });

                // Create a shuffled list of spawn indices [0..min(3, spawns.size()-1)] then reuse if needed.
                std::vector<std::uint8_t> availableSpawnIdx;
                const std::size_t maxUnique = std::min<std::size_t>(4, spawns.size());
                availableSpawnIdx.reserve(maxUnique);
                for (std::size_t i = 0; i < maxUnique; ++i) {
                    availableSpawnIdx.push_back(static_cast<std::uint8_t>(i));
                }
                static std::mt19937 spawnGen{ std::random_device{}() };
                std::shuffle(availableSpawnIdx.begin(), availableSpawnIdx.end(), spawnGen);

                // spawnPoints[i] corresponds to playersSorted[i] (sorted by playerId)
                std::vector<std::uint8_t> spawnPoints;
                spawnPoints.reserve(playersSorted.size());
                for (std::size_t i = 0; i < playersSorted.size(); ++i) {
                    const std::uint8_t spawnIdx = availableSpawnIdx[i % availableSpawnIdx.size()];
                    spawnPoints.push_back(spawnIdx);
                    playersSorted[i]->setPosition(spawns[spawnIdx]);
                    playersSorted[i]->setVelocity({0.f, 0.f});
                }

                // Start a short time in the future so clients can load stage + apply spawns.
                const TICK_TYPE gameStartTick = currentTick + sf::seconds(mapStartTimeS)/tickRate;

                gameStates[currentTick].setStage(selectedStage);
                gameStates[currentTick].setGameStartTick(gameStartTick);
                gameStates[currentTick].setGameState(gameState::STARTING);

                EventStartGame eventStartGame(gameStartTick, chosenStageId, spawnPoints);
                for (std::unique_ptr<ServerConnection> &connPtr : serverSocket.connections) {
                    connPtr->sendTcpEvent(eventStartGame);
                }

                // Done with map selection for this round.
                selectedMaps.byPlayer.clear();
                selectedMaps.arrivalOrder.clear();
            }
        }


        auto& players = gameStates[currentTick].getPlayers();
        int aliveCount = 0;
        Player* lastAlive = nullptr;
        for (auto& p : players) {
            if (p.getHealth() > 0) {
                aliveCount++;
                lastAlive = &p;
            }
        }

        if (gameStates[currentTick].getGameState() == gameState::RUNNING && aliveCount == 1 && lastAlive) {
            lastAlive->setScore(lastAlive->getScore() + 1); // or use a setter
            // Notify clients: lastAlive->getId() won the round
            if (lastAlive->getScore() >= 10) {
                // Notify clients: lastAlive->getId() won the game
                // Optionally reset scores, return to lobby, etc.
            }
        }

        //end of round
        //TODO: when round is finished send out a ENDOFROUND event wait 5 seconds and then send out a START event.
        if (gameStates[currentTick].getGameState() == gameState::RUNNING && aliveCount <= 1) {
            auto& gs = gameStates[currentTick];
            const auto& spawns = gs.getStage().getSpawnPoints();
            const int16_t stageId = gs.getStage().getStageId();

            // Assign spawns deterministically by sorting players by id.
            std::vector<Player*> playersSorted;
            playersSorted.reserve(gs.getPlayers().size());
            for (auto& p : gs.getPlayers()) {
                playersSorted.push_back(&p);
            }
            std::sort(playersSorted.begin(), playersSorted.end(), [](const Player* a, const Player* b) {
                return a->getId() < b->getId();
            });

            std::vector<std::uint8_t> spawnPoints;
            spawnPoints.reserve(playersSorted.size());

            if (spawns.empty()) {
                PLOG_ERROR << "Cannot reset round: stage has no spawn points";
            } else {
                std::vector<std::uint8_t> availableSpawnIdx;
                const std::size_t maxUnique = std::min<std::size_t>(4, spawns.size());
                availableSpawnIdx.reserve(maxUnique);
                for (std::size_t i = 0; i < maxUnique; ++i) {
                    availableSpawnIdx.push_back(static_cast<std::uint8_t>(i));
                }
                static std::mt19937 spawnGen{ std::random_device{}() };
                std::shuffle(availableSpawnIdx.begin(), availableSpawnIdx.end(), spawnGen);

                for (std::size_t i = 0; i < playersSorted.size(); ++i) {
                    const std::uint8_t spawnIdx = availableSpawnIdx[i % availableSpawnIdx.size()];
                    spawnPoints.push_back(spawnIdx);

                    playersSorted[i]->setHealth(10.f);
                    playersSorted[i]->setVelocity({0.f, 0.f});
                    playersSorted[i]->setProjectileCooldown(0);
                    playersSorted[i]->setPosition(spawns[spawnIdx]);
                }
            }

            // Deactivate all projectiles so clients don't see leftover shots in the next round.
            for (auto& pr : gs.getProjectiles()) {
                pr.setIsActive(false);
                pr.setVelocity({0.f, 0.f});
            }

            respawnAtTick.clear();

            const TICK_TYPE nextStartTick = currentTick + 600;
            gs.setGameStartTick(nextStartTick);
            gs.setGameState(gameState::STARTING);

            serverSocket.sendTcpEventToEveryone(EventStartGame(nextStartTick, stageId, spawnPoints));
        }

        #if ENABLE_SERVER_RENDERING
        renderer.render(gameStates[currentTick]);
        renderer.processDisplayEvents();
        renderer.display();
        #endif

        someTimesResyncGameState();
        sf::Time tickComputeTime = tickClock.getElapsedTime() - startTickTime;
        if(CONF_SEND_SERVER_HEALTH){
            sendServerHealth(tickComputeTime);
        }
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
                if(evLoginRequest->apiVersion != CONF_API_VERSION){
                    conn.sendTcpEvent(EventLoginDenied(1));
                    continue;
                }
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
            PLOG_DEBUG_IF(debugServerNetworking) << "received user input";
            while (evUserInput->hasNextUserInput())
            {
                auto input = evUserInput->getNextUserInput();
                PLOG_DEBUG_IF(debugServerInputProcessing) << "adding user input '" << input.idx << "' to queue";
                conn.enqueueInput(input);
            }
        }

        EventSelectedMap* evSelectedMap = dynamic_cast<EventSelectedMap*>(ev);
        if (evSelectedMap != nullptr) {
            if (gameStates[currentTick].getGameState() == gameState::MAP_SELECT) {
                const OBJECT_ID_TYPE pid = conn.getPlayerId();
                if (selectedMaps.byPlayer.find(pid) == selectedMaps.byPlayer.end()) {
                    selectedMaps.arrivalOrder.push_back(pid);
                }
                selectedMaps.byPlayer[pid] = evSelectedMap->stageId;
                PLOG_DEBUG_IF(debugServerNetworking) << "Received [Event] Selected Map from player " << pid << ": stageId=" << evSelectedMap->stageId;
            }
        }

        eventData.connectionEventsQueue.pop();
    }
}

void Server::someTimesResyncGameState(){
    #define PLAYERRESYNCTIMER 3
    static unsigned tickCounter = 1;
    tickCounter--;
    if(tickCounter == 0){
        tickCounter = PLAYERRESYNCTIMER;
        resyncGameState();
    }else{
        resyncLastInputs();
    }
}

void Server::sendServerHealth(sf::Time frameTime){
    for (auto& conn : serverSocket.connections)
    {
        conn->sendUdpEvent(EventServerHealth((HEALTH_INPUT_QUEUE_TYPE) conn->getInputQueueSize(),(HEALTH_FRAME_TIME_TYPE) frameTime.asMilliseconds()));
    }
    
}

void Server::resyncGameState(){
    // perspective: this functions should send the most recent gameState
    // along with every movement in between the gs and the previous one(and perhaps even one before for safety)
    // so that clients will be able to correctly interfer gameStates between these  
    // that means we have to include all current data and all new inputs since the last 2 synced gameStates
    
    static constexpr int SNAPSHOT_DISTANCE = 5;
    static constexpr int MAX_TOTAL_INFOS_TO_SEND = SNAPSHOT_DISTANCE + 1;

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
            PLOG_DEBUG_IF(debugServerNetworking) << "tick(current, currentToSend, i): " << currentTick << ' ' << currentTickToSend << ' ' << i;
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

                PLOG_VERBOSE_IF(debugServerNetworking) << "server: playerInfosSize: " << syncEvent.updateInfos.back().gsUpdate.playerInfos.size();
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
        try{
            connPtr->sendUdpEvent(syncEvent);
        }catch(std::runtime_error){
            PLOG_WARNING_IF(debugServerNetworking) << "error sending udp sync to " << connPtr->udpRecipientIpAdress << ':' << connPtr->udpRecipientPort;
        }
        /* code */
        PLOG_DEBUG_IF(debugServerNetworking) << "sending " << syncEvent.updateInfos.size() << " updates at starting tick " << syncEvent.startingGameTick;
    }
}

void Server::resyncLastInputs(){
    static constexpr int MAX_TOTAL_INFOS_TO_SEND = 3;

    int infosToSend = MAX_TOTAL_INFOS_TO_SEND;
    //current Tick = 1 == can send 2 ticks
    if(currentTick + 1 < MAX_TOTAL_INFOS_TO_SEND){
        infosToSend = currentTick + 1;
    }
    
    for (std::unique_ptr<ServerConnection> &connPtr : serverSocket.connections)
    {
        EventGamestatePlayerInputHistory syncEvent = EventGamestatePlayerInputHistory();
        syncEvent.startingGameTick = currentTick + 1 - infosToSend;
        syncEvent.snapShotDistance = 255;

        for (TICK_TYPE i = 0; i < infosToSend; i++){
            TICK_TYPE currentTickToSend = syncEvent.startingGameTick + i;
            PLOG_DEBUG_IF(debugServerNetworking) << "tick(current, currentToSend, i): " << currentTick << ' ' << currentTickToSend << ' ' << i;

            auto& eventInputs = syncEvent.createNewPlayerInputs();
            eventInputs.reserve(inputHistory[currentTickToSend].size());
            for (auto& pInput : inputHistory[currentTickToSend])
            {
                eventInputs.emplace_back(pInput.playerInputWithId);
            }
        }
        
        //set latestUpdatedInputSync
        try{
            connPtr->sendUdpEvent(syncEvent);
        }catch(std::runtime_error){
            PLOG_WARNING_IF(debugServerNetworking) << "error sending udp sync to " << connPtr->udpRecipientIpAdress << ':' << connPtr->udpRecipientPort;
        }
        /* code */
        PLOG_DEBUG_IF(debugServerNetworking) << "sending " << syncEvent.updateInfos.size() << " updates at starting tick " << syncEvent.startingGameTick;
    }
}