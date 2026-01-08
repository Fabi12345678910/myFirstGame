#include "Client.h"
#include "PlayerOperations.h"
#include "Inputs.h"
#include "Projectile.h"
#include "GameUpdate.h"
#include "Renderer.h"
#include "plog/Log.h"
#include "Operations/ClientGameStateUpdater.h"
#include "StageManager.h"

#include "Networking/EventDefinitions/EventLoginRequest.h"
#include "Networking/EventDefinitions/EventLoginDenied.h"
#include "Networking/EventDefinitions/EventLoginConfirmation.h"
#include "Networking/EventDefinitions/EventPlayerLocation.h"
#include "Networking/EventDefinitions/EventPlayerVelocity.h"
#include "Networking/EventDefinitions/EventSpawnNewPlayer.h"
#include "Networking/EventDefinitions/EventUserInput.h"
#include "Networking/EventDefinitions/EventGamestatePlayerInputHistory.h"
#include "Networking/EventDefinitions/EventSelectMap.h"
#include "Networking/EventDefinitions/EventSelectedMap.h"
#include "Networking/EventDefinitions/EventStartGame.h"

#include "maps/Map_TestAll.h"

void* clientEventHandler(std::unique_ptr<Event> evPtr, Connection& conn, void* args) {
    struct clientEventHandlerData *handle = (clientEventHandlerData*) args;
    std::lock_guard<std::mutex> queueLockGuard(handle->connectionEventsMutex);
    handle->connectionEventsQueue.push(std::move(evPtr));
    return NULL;

};

void* udpClientEventHandler(std::unique_ptr<Event> evPtr, std::optional<sf::IpAddress> &remoteAddress, unsigned short &remotePort, void *args){
    PLOG_VERBOSE << "got udp event";
    struct clientEventHandlerData *handle = (clientEventHandlerData*) args;
    std::lock_guard<std::mutex> queueLockGuard(handle->connectionEventsMutex);
    handle->connectionEventsQueue.push(std::move(evPtr));
    return NULL;
}


Client::Client(sf::RenderWindow& win)
        : renderer(win),
            conn(ClientConnection::createClientConnection({127, 0, 0, 1}, 4444)),
            isHost(isHost)
{
    conn.setArgs(&eventData);
    conn.setEventHandler(clientEventHandler);
    performLogin();
}

// constructor with specified port
Client::Client(sf::RenderWindow& win, sf::IpAddress ip, unsigned short port)
        : renderer(win),
            conn(ClientConnection::createClientConnection(ip, port)),
            isHost(isHost)
{
    sf::View worldView;
    worldView.setSize(sf::Vector2f(1920, 1080));       // virtual world size
    worldView.setCenter(sf::Vector2f(1920.f / 2.f, 1080.f / 2.f));
    win.setView(worldView);

    conn.setArgs(&eventData);
    conn.setEventHandler(clientEventHandler);
    performLogin();
}

void Client::performLogin(){
    conn.sendTcpEvent(EventLoginRequest(conn.getUdpPort()));
    while (true)
    {
        std::lock_guard<std::mutex> queueLockGuard(eventData.connectionEventsMutex);
        while(!eventData.connectionEventsQueue.empty()){
            auto& evPtr = eventData.connectionEventsQueue.front();
            Event* ev = evPtr.get();
            EventLoginConfirmation* evLoginSuccess = dynamic_cast<EventLoginConfirmation*>(ev);
            if(evLoginSuccess != NULL){
                this->playerId = evLoginSuccess->playerId;
                this->tickrateMs = evLoginSuccess->serverTickRateMs;
                tickToDisplay = evLoginSuccess->latestServerTick - displayTickDifference;
                if(evLoginSuccess->latestServerTick < displayTickDifference){
                    throw std::runtime_error("unable to initiate entity interpolation");
                }
                clientState = AWAITING_SPAWN;
                conn.setUdpArgs(&eventData);
                conn.setUdpEventHandler(udpClientEventHandler);
                return;
            }
            EventLoginDenied*evLoginDenied  = dynamic_cast<EventLoginDenied*>(ev);
            if(evLoginDenied != NULL){
                throw std::runtime_error("Login denied");
            }

            // Ignore unrelated events during login to avoid blocking on a non-login packet.
            eventData.connectionEventsQueue.pop();
        }
    }
    sf::sleep(sf::milliseconds(20));
}

void Client::run(){

    GameState baseGameState = GameState();
    Stage lobbyStage = StageManager::loadStage(1);
    baseGameState.setStage(lobbyStage);

    gameStore = ClientGameStateStore<clientGameStateBufferSize>(1, baseGameState);
    gameStore.setTickrate(sf::milliseconds(this->tickrateMs).asSeconds());
    gameStore.setLocalPlayerId(this->playerId);

    //enter the main loop
    mainLoop();
}

void Client::processEventsAwaitingSpawn(){

    std::lock_guard<std::mutex> queueLockGuard(eventData.connectionEventsMutex);
    while(!eventData.connectionEventsQueue.empty()){
        auto& evPtr = eventData.connectionEventsQueue.front();
        Event* ev = evPtr.get();
        //somehow handle tha event
        EventSpawnNewPlayer *evSpawnNewPlayer = dynamic_cast<EventSpawnNewPlayer*>(ev);
        if(evSpawnNewPlayer != NULL){
            // Only start playing once *we* have spawned.
            if(evSpawnNewPlayer->playerId == this->playerId){
                clientState = PLAYING;
            }
//TODO store player in GameStore.addPlayer()
/*            gameStates[latestGeneratedTick].gameState.addPlayer(Player(evSpawnNewPlayer->playerId, sf::Vector2f(40.f, 40.f),evSpawnNewPlayer->location));
            if(evSpawnNewPlayer->playerId == this->playerId){
                // we have spawned and can now start the game
                clientState = PLAYING;
                eventData.connectionEventsQueue.pop();
                return;
            }*/
        }
        eventData.connectionEventsQueue.pop();
    }
}

void Client::processEventsPlaying(){

    std::lock_guard<std::mutex> queueLockGuard(eventData.connectionEventsMutex);
    while(!eventData.connectionEventsQueue.empty()){
        auto& evPtr = eventData.connectionEventsQueue.front();
        Event* ev = evPtr.get();
        EventGamestatePlayerInputHistory *eventGamestatePlayerInputHistory = dynamic_cast<EventGamestatePlayerInputHistory*>(ev);
        if(eventGamestatePlayerInputHistory != NULL){
            PLOG_VERBOSE << "handling gameStateUpdate";
            updateGameStates(*eventGamestatePlayerInputHistory);
        }

        EventSelectMap* eventSelectMap = dynamic_cast<EventSelectMap*>(ev);
        if(eventSelectMap != nullptr){
            PLOG_DEBUG << "Received [Event] Select Map";
            mapSelectionState.selectUntil = eventSelectMap->selectMapUntil;
            gameStore.getGameState(tickToDisplay, true, NULL)->setGameState(gameState::MAP_SELECT);
        }

        EventStartGame* eventStartGame = dynamic_cast<EventStartGame*>(ev);
        if (eventStartGame != nullptr) {
            PLOG_ERROR << "Received [Event] Start Game (stageId=" << eventStartGame->stageId << ")";
            Stage selectedStage = StageManager::loadStage(eventStartGame->stageId);
            if (GameState* gs = gameStore.getGameState(tickToDisplay, true, NULL)) {
                gs->setGameStartTick(eventStartGame->gameStartTick);
                gs->setStage(selectedStage);
                gs->setGameState(gameState::STARTING);
            }
        }

        eventData.connectionEventsQueue.pop();
    }
}

//local GameStateAddons{
//  uint32_t localInputId; //this might just be the client tick
//  Player   localPlayer;
//  inputs   clientInputs
//}
void Client::updateGameStates(EventGamestatePlayerInputHistory& ev){
    TICK_TYPE currentTickInfo = ev.startingGameTick;
    while (ev.hasNextInfo())
    {
        LabeledUpdateInfo update = ev.getNextInfo();
        for(auto &pInput : update.info.pInput){
            gameStore.addUpdateInfo(currentTickInfo, pInput);
        }
        if(update.type == UpdateInfo1::GAMESTATE_PLAYER_INPUT){
            gameStore.addUpdateInfo(currentTickInfo, update.info.gsUpdate);
        }
        gameStore.finalizeUpdateInfos(currentTickInfo);
        currentTickInfo++;
    }
}

void Client::mainLoop(){
    PLOG_INFO << "entering main loop";
    sf::Clock tickClock;
    tickClock.start();
    sf::Time startTime = tickClock.getElapsedTime();
    sf::Time tickRate = sf::milliseconds(tickrateMs);
    sf::Time latestElapsedTick = startTime;
    TICK_TYPE displayedTicks;

    float loadingAngle = 0.f;

    int dotFrame = 0;
    sf::Clock dotClock;

    while(true){
        if(clientState == PLAYING){
            sf::Time startTickTime = tickClock.getElapsedTime();
            sf::Time currentDeltaTime = startTickTime - latestElapsedTick;

            processEventsPlaying();

            GameState* generatedGameState = NULL;
            Player* localPlayer = NULL;
            bool canRenderTick = true;
            
            playerInput input;
            if (lastRenderedGameState != gameState::STARTING) {
                input = processInputs();
            }
            PLOG_VERBOSE << "delta time " << currentDeltaTime.asSeconds();
            PLOG_VERBOSE << "tickrate time " << tickRate.asSeconds();
            while (currentDeltaTime >= tickRate){
                tickToDisplay++;
        
                PLOG_DEBUG << "attempting to generate display tick " << tickToDisplay;
                currentDeltaTime-= tickRate;
                PLOG_VERBOSE << "remaining delta time " << currentDeltaTime.asSeconds();
                gameStore.setLocalInput(tickToDisplay, input);
                gameStore.finalizeLocalInput(tickToDisplay);
                latestElapsedTick += tickRate;
                constexpr size_t maxHistoricInputsToSend = 8;
                EventUserInput userInputs(this->playerId);
                TICK_TYPE startTick = 0;
                if (tickToDisplay > maxHistoricInputsToSend) {
                    startTick = tickToDisplay - static_cast<TICK_TYPE>(maxHistoricInputsToSend);
                }
                for (TICK_TYPE i = startTick; i <= tickToDisplay; i++)
                {
                    if(gameStore.hasLocalInput(i)){
                        userInputs.addUserInput(indexedPlayerInput(i, gameStore.getLocalInput(i)));
                    }
                }
                conn.sendUdpEvent(userInputs);
            }

            generatedGameState = gameStore.getGameState(tickToDisplay, true, &localPlayer);  

            if(generatedGameState != NULL){
                lastRenderedGameState = generatedGameState->getGameState();
                GameState displayGameState = *generatedGameState;
                const bool hasLocalPrediction = (localPlayer != NULL);
                bool localAliveOnServer = true;
                try {
                    localAliveOnServer = (generatedGameState->getPlayer(this->playerId).getHealth() > 0);
                } catch (const std::exception&) {
                    // If we don't have the local player yet (e.g., joining mid-stream),
                    // fall back to rendering whatever is in the authoritative gamestate.
                    localAliveOnServer = true;
                }

                if (hasLocalPrediction && localAliveOnServer) {
                    Player locPlayer = *localPlayer;
                    locPlayer.getShape().setFillColor(sf::Color::Magenta);
                    displayGameState.addPlayer(locPlayer);
                }

                constexpr bool renderLocalServerPlayer = false;
                if (!renderLocalServerPlayer && hasLocalPrediction && localAliveOnServer){
                    displayGameState.removePlayer(this->playerId);
                }


                for (auto& player :displayGameState.getPlayers())
                {
                    PLOG_VERBOSE << "incl. player: " << player.getId() << '\n';
                }
                
                renderer.render(displayGameState);
                
                // ─── State Handling ───────────────────────────────────────────────

                if (displayGameState.getPlayers().size() < 2) {
                    if(dotClock.getElapsedTime().asMilliseconds() > 800){
                        dotFrame = (dotFrame % 3) + 1;
                        dotClock.restart();
                    }
                    renderer.renderWaitingMessage(dotFrame);
                }
                else if (displayGameState.getGameState() == gameState::WAITING) {
                        bool readyToPlay = false;
                        try {
                            readyToPlay = generatedGameState->getPlayer(playerId).getReadyToPlay();
                        } catch (const std::exception&) {
                            // Joining mid-stream: local player may not be in the buffered gamestate yet.
                            readyToPlay = false;
                        }
                        renderer.renderReadyMessage(readyToPlay);
                }
                else if (displayGameState.getGameState() == gameState::MAP_SELECT) {
                    if (tickToDisplay <= mapSelectionState.selectUntil) {
                        renderer.renderMapSelection(mapSelectionState.selectUntil - tickToDisplay, mapSelectionState.selectedIndex, mapSelectionState.confirmed, mapSelectionState.maps);
                    }
                    else {
                        if (mapSelectionState.confirmed) {
                            if (mapSelectionState.selectedIndex >= 0 &&
                                static_cast<size_t>(mapSelectionState.selectedIndex) < mapSelectionState.maps.size()) {
                                mapSelectionState.selectedStageId = mapSelectionState.maps[mapSelectionState.selectedIndex].first;
                            }
                        }
                        if (mapSelectionState.selectedStageId != -1) {
                            EventSelectedMap eventSelectedMap(mapSelectionState.selectedStageId);
                            conn.sendTcpEvent(eventSelectedMap);
                        } 
                        //displayGameState.setGameState(gameState::LOADING);
                        if (generatedGameState != NULL) {
                            generatedGameState->setGameState(gameState::LOADING);
                        }
                        renderer.renderLoading(loadingAngle);
                        loadingAngle++;
                    }
                }
                else if (displayGameState.getGameState() == gameState::LOADING) {
                    renderer.renderLoading(loadingAngle);
                    loadingAngle++;
                }
                else if (displayGameState.getGameState() == gameState::STARTING) {
                    TICK_TYPE gameStartTick = gameStore.getGameState(tickToDisplay, true, NULL)->getGameStartTick();
                    if (tickToDisplay < gameStartTick) {
                        renderer.renderGameStart(gameStartTick - tickToDisplay);
                    }
                    else {
                        if (generatedGameState != NULL) {
                            generatedGameState->setGameState(gameState::RUNNING);
                        }
                        lastRenderedGameState = gameState::RUNNING;
                    }
                }
                else if (displayGameState.getGameState() == gameState::RUNNING) {

                }
                auto healthReport = gameStore.getHealthReport(tickToDisplay);
                renderer.renderGameStateHealth(healthReport);
                renderer.processDisplayEvents();
                renderer.display();
            }else{
                PLOG_INFO << "can't render tick:(\n";
            }
            sf::Time tickRenderTime = tickClock.getElapsedTime() - startTickTime;
            if(tickRenderTime.asMilliseconds() >= 5){
                PLOG_INFO << "Computing tick took " << tickRenderTime.asMilliseconds() << "ms";
            }

//            sf::sleep(tickRate - tickClock.getElapsedTime());
        }
        else if(clientState == AWAITING_SPAWN){
            PLOG_DEBUG << "be awaiting spawn";
            processEventsAwaitingSpawn();
            sf::sleep(sf::milliseconds(10));
        }
    }
    PLOG_INFO << "exiting main loop";
}

playerInput Client::processInputs(){
    playerInput input;
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)){
        input.moveLeft = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)){
        input.moveRight = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
        input.jump = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::J)) {
        input.projectile = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
        input.readyToPlay = true;
    }

    return input;
}