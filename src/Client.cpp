#include "Client.h"
#include "Inputs.h"
#include "GameUpdate.h"
#include "Renderer.h"
#include "Types.h"
#include "plog/Log.h"
#include "StageManager.h"

#include "Networking/EventDefinitions/EventLoginRequest.h"
#include "Networking/EventDefinitions/EventLoginDenied.h"
#include "Networking/EventDefinitions/EventLoginConfirmation.h"
#include "Networking/EventDefinitions/EventSpawnNewPlayer.h"
#include "Networking/EventDefinitions/EventUserInput.h"
#include "Networking/EventDefinitions/EventGamestatePlayerInputHistory.h"
#include "Networking/EventDefinitions/EventSelectMap.h"
#include "Networking/EventDefinitions/EventSelectedMap.h"
#include "Networking/EventDefinitions/EventStartGame.h"
#include "Networking/EventDefinitions/EventServerHealth.h"

#include "Config.h"
#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include <stdexcept>


void* clientEventHandler(std::unique_ptr<Event> evPtr, Connection& conn, void* args) {
    struct clientEventHandlerData *handle = (clientEventHandlerData*) args;
    std::lock_guard<std::mutex> queueLockGuard(handle->connectionEventsMutex);
    handle->connectionEventsQueue.push(std::move(evPtr));
    return NULL;

};

void* udpClientEventHandler(std::unique_ptr<Event> evPtr, std::optional<sf::IpAddress> &remoteAddress, unsigned short &remotePort, void *args){
    PLOG_VERBOSE_IF(debugClientNetworking) << "got udp event";
    struct clientEventHandlerData *handle = (clientEventHandlerData*) args;
    std::lock_guard<std::mutex> queueLockGuard(handle->connectionEventsMutex);
    handle->connectionEventsQueue.push(std::move(evPtr));
    return NULL;
}


Client::Client(sf::RenderWindow& win, bool isHost)
        : renderer(win),
            conn(ClientConnection::createClientConnection({127, 0, 0, 1}, 4444)),
            isHost(isHost)
{
    conn.setArgs(&eventData);
    conn.setEventHandler(clientEventHandler);
    performLogin();
}

// constructor with specified port
Client::Client(sf::RenderWindow& win, sf::IpAddress ip, unsigned short port, bool isHost)
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
                clientState = C_LOBBY;
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
                clientState = C_LOBBY;
            }
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
            PLOG_VERBOSE_IF(debugClientNetworking) << "handling gameStateUpdate";
            updateGameStates(*eventGamestatePlayerInputHistory);
        }

        EventSelectMap* eventSelectMap = dynamic_cast<EventSelectMap*>(ev);
        if(eventSelectMap != nullptr){
            PLOG_DEBUG_IF(debugClientNetworking) << "Received [Event] Select Map";
            mapSelectionState.selectUntil = eventSelectMap->selectMapUntil;
            this->clientState = C_MAP_SELECTION;
        }

        EventStartGame* eventStartGame = dynamic_cast<EventStartGame*>(ev);
        if (eventStartGame != nullptr) {
            PLOG_INFO_IF(debugClientNetworking) << "Received [Event] Start Game (stageId=" << eventStartGame->stageId << ")";
            Stage selectedStage = StageManager::loadStage(eventStartGame->stageId);
            this->clientState = C_COUNTDOWN;
            this->gameStartTick = eventStartGame->gameStartTick;
            this->gameStore.loadStage(selectedStage);
        }

        EventServerHealth *eventServerHealth = dynamic_cast<EventServerHealth*>(ev);
        if(eventServerHealth != NULL){
            PLOG_VERBOSE_IF(debugClientNetworking) << "got a server health";
            this->serverQueueHealth = eventServerHealth->inputsInQueue;
            this->serverFrameTimes.push(eventServerHealth->frameTimeMs);
        }

        EventSpawnNewPlayer *evSpawnNewPlayer = dynamic_cast<EventSpawnNewPlayer*>(ev);
        if(evSpawnNewPlayer != NULL){
            //unused as of now, will be synced at gsUpdate
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
        PLOG_VERBOSE_IF(debugClientNetworking) << "storing updateInfos at tick: " << currentTickInfo;
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

void Client::storeInputs(const TICK_TYPE& startingTick, const TICK_TYPE& currentTick, playerInput input){
    for (TICK_TYPE tick = startingTick; tick <= currentTick; tick++) {
        if(input.readyToPlay){
            if(!isReadyForSelection()){
                input.readyToPlay = false;
            }else{
                selectedSomething();
            }
        }
        gameStore.setLocalInput(tick, input);
        gameStore.finalizeLocalInput(tick);
    }
}

void Client::sendInputs(const TICK_TYPE& startingTick, const TICK_TYPE& currentTick){
    constexpr size_t maxHistoricInputsToSend = 3;
    for (TICK_TYPE tick = startingTick; tick <= currentTick; tick++) {
        EventUserInput inputs(this->playerId);
        for (TICK_TYPE tickInputToInclude = tick - maxHistoricInputsToSend + 1; tickInputToInclude <= currentTick; tickInputToInclude++) {
            if(gameStore.hasLocalInput(tickInputToInclude)){
                inputs.addUserInput(indexedPlayerInput(tickInputToInclude, gameStore.getLocalInput(tickInputToInclude)));
            }
        }
        conn.sendUdpEvent(inputs);
    }
}

void Client::processMapSelectionInputs(MapSelectionInput input){
    if(this->mapSelectionState.confirmed){return;}
    if(!isReadyForSelection()){
        return;
    }
    if(input.goLeft){
        mapSelectionState.selectedIndex = (mapSelectionState.selectedIndex + 1) % mapSelectionState.maps.size();
        selectedSomething();
    }
    if(input.goRight){
        mapSelectionState.selectedIndex = (mapSelectionState.selectedIndex - 1) % mapSelectionState.maps.size();
        selectedSomething();
    }
    if(input.confirm){
        EventSelectedMap eventSelectedMap(mapSelectionState.maps[mapSelectionState.selectedIndex].first);
        conn.sendTcpEvent(eventSelectedMap);
        mapSelectionState.confirmed = true;
        selectedSomething();
    }
}

void Client::renderStateSpecificInfo(bool readyToPlay){
    if (this->clientState == C_LOBBY) {
//        TODO make some state PLAYING->READY_SELECTION
//        renderer.renderWaitingMessage();
        renderer.renderReadyMessage(readyToPlay);
    }
    else if (this->clientState == C_MAP_SELECTION) {
        renderer.renderMapSelection(mapSelectionState.selectUntil - tickToDisplay, mapSelectionState.selectedIndex, mapSelectionState.confirmed, mapSelectionState.maps);
    }
    else if (this->clientState == C_WAITING_FOR_COUNTDOWN) {
        renderer.renderLoading();
    }
    else if (this->clientState == C_COUNTDOWN) {
        renderer.renderGameStart(gameStartTick - tickToDisplay);
    }
}

void Client::mainLoop(){
    PLOG_INFO << "entering main loop";
    sf::Clock tickClock;
    tickClock.start();
    sf::Time startTime = tickClock.getElapsedTime();
    sf::Time tickRate = sf::milliseconds(tickrateMs);
    sf::Time latestProcessedInputsTick = startTime;
    TICK_TYPE displayedTicks;

    float loadingAngle = 0.f;

    int dotFrame = 0;
    sf::Clock dotClock;

    while(true){
        sf::Time startTickTime = tickClock.getElapsedTime();
        sf::Time currentDeltaTime = startTickTime - latestProcessedInputsTick;

        TICK_TYPE prevDisplayedTick = tickToDisplay;
        tickToDisplay += (TICK_TYPE) (currentDeltaTime / tickRate);

        #define LOG_TIMEPOINT(name) PLOG_DEBUG_IF(debugClientPerformance) << "timepoint: " << name << ", elapsed tickTime: " <<  (tickClock.getElapsedTime() - startTickTime).asMicroseconds() << "us";
        PLOG_VERBOSE_IF(debugClientFrameGen) << "delta time " << currentDeltaTime.asSeconds();
        PLOG_VERBOSE_IF(debugClientFrameGen) << "tickrate time " << tickRate.asSeconds();

        PLOG_DEBUG_IF(debugClientPerformance) << "--- starting frame ---";
        PLOG_DEBUG_IF(debugClientState) << "clientState: " << clientState;
        processEventsPlaying();
        if(clientState == C_LOBBY){
            LOG_TIMEPOINT("processedEvents");
            storeInputs(prevDisplayedTick + 1, tickToDisplay, processInputs());
            sendInputs(prevDisplayedTick + 1, tickToDisplay);
            LOG_TIMEPOINT("processed playerInputs");
        }
        else if(clientState == C_MAP_SELECTION){
            MapSelectionInput input = processInputsMapSelection();
            processMapSelectionInputs(input);
            storeInputs(prevDisplayedTick + 1, tickToDisplay, playerInput());
            if(tickToDisplay > mapSelectionState.selectUntil){
                clientState = C_WAITING_FOR_COUNTDOWN;
            }
        }
        else if(clientState == C_WAITING_FOR_COUNTDOWN){
            storeInputs(prevDisplayedTick + 1, tickToDisplay, playerInput());
            if(tickToDisplay >= gameStartTick - 1){
                clientState = C_GAME_RUNNING;
            }
        }
        else if(clientState == C_COUNTDOWN){
            storeInputs(prevDisplayedTick + 1, tickToDisplay, playerInput());
            if(tickToDisplay >= gameStartTick - 1){
                clientState = C_GAME_RUNNING;
            }
        }else if (clientState == C_GAME_RUNNING){
            storeInputs(prevDisplayedTick + 1, tickToDisplay, processInputs());
            sendInputs(prevDisplayedTick + 1, tickToDisplay);
        }
        latestProcessedInputsTick += tickRate * (std::int64_t)(tickToDisplay-prevDisplayedTick);

        Player* localPlayer = NULL;
        PLOG_VERBOSE_IF(debugClientFrameGen) << "attempting to generate gameState at tick: " << tickToDisplay;
        GameState* generatedGameState = gameStore.getGameState(tickToDisplay, true, &localPlayer);  
        LOG_TIMEPOINT("generated gamestate");

        if(generatedGameState != NULL){
            //do interpolation
            GameState interpolatedGameState = *generatedGameState;
            //TODO actually perform interpolation here
            if(localPlayer != NULL){
                constexpr bool renderLocalServerPlayer = false;
                if(!renderLocalServerPlayer){
                    interpolatedGameState.removePlayer(this->playerId);
                }
                localPlayer->getShape().setFillColor(sf::Color::Magenta);
                interpolatedGameState.addPlayer(*localPlayer);
            }
            for (auto& player :interpolatedGameState.getPlayers())
            {
                PLOG_VERBOSE_IF(debugClientFrameGen) << "incl. player: " << player.getId() << '\n';
            }

            bool readyToPlay = false;
            try {
                readyToPlay = generatedGameState->getPlayer(this->playerId).getReadyToPlay();
            } catch (std::runtime_error e) {
            }
            
            LOG_TIMEPOINT("prepared interpolatedGameState");
            renderer.render(interpolatedGameState);
            LOG_TIMEPOINT("rendered interpolatedGameState");
            renderStateSpecificInfo(readyToPlay);
            LOG_TIMEPOINT("rendered statespecific info");
            

            if (CONF_SHOW_CLIENT_HEALTH){
                auto healthReport = gameStore.getHealthReport(tickToDisplay);
                renderer.renderFrameTimeGraph(clientFrameTimes, (HEALTH_FRAME_TIME_TYPE) tickrateMs, 50.F, 450.F);
                renderer.renderGameStateHealth(healthReport);
                LOG_TIMEPOINT("rendered client health");
            }
            if(CONF_SHOW_SERVER_HEALTH){
                renderer.renderServerQueueHealth(serverQueueHealth);
                renderer.renderFrameTimeGraph(serverFrameTimes, (HEALTH_FRAME_TIME_TYPE) tickrateMs, 50.F, 350.F);
                LOG_TIMEPOINT("rendered server health");
            }

            renderer.display();

            sf::Time tickRenderTime = tickClock.getElapsedTime() - startTickTime;
            clientFrameTimes.push((HEALTH_FRAME_TIME_TYPE) tickRenderTime.asMilliseconds());
            if(tickRenderTime > tickRate){
                PLOG_ERROR << "Computing tick took " << tickRenderTime.asMilliseconds() << "ms, which is slower than the server tickrate";
            }
            LOG_TIMEPOINT("finished frame");
        }else{
            PLOG_DEBUG << "can't render tick " << tickToDisplay;
        }
        renderer.processDisplayEvents();
        #undef LOG_TIMEPOINT
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

MapSelectionInput Client::processInputsMapSelection(){
    using namespace sf::Keyboard;
    MapSelectionInput input;
    if (isKeyPressed(Key::Down) || isKeyPressed(Key::Left)){
        input.goLeft = true;
    }
    else if (isKeyPressed(Key::Up) || isKeyPressed(Key::Right)){
        input.goRight = true;
    }
    if(isKeyPressed(Key::Enter)){
        input.confirm = true;
    }
    return input;
}