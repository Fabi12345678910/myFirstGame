#include "Client.h"
#include "PlayerOperations.h"
#include "Inputs.h"
#include "Projectile.h"
#include "GameUpdate.h"
#include "Renderer.h"
#include "Operations/ClientGameStateUpdater.h"

#include "Networking/EventDefinitions/EventLoginRequest.h"
#include "Networking/EventDefinitions/EventLoginDenied.h"
#include "Networking/EventDefinitions/EventLoginConfirmation.h"
#include "Networking/EventDefinitions/EventPlayerLocation.h"
#include "Networking/EventDefinitions/EventPlayerVelocity.h"
#include "Networking/EventDefinitions/EventSpawnNewPlayer.h"
#include "Networking/EventDefinitions/EventUserInput.h"
#include "Networking/EventDefinitions/EventGamestatePlayerInputHistory.h"

#include "maps/Map_TestAll.h"

void* clientEventHandler(std::unique_ptr<Event> evPtr, Connection& conn, void* args) {
    struct clientEventHandlerData *handle = (clientEventHandlerData*) args;
    std::lock_guard<std::mutex> queueLockGuard(handle->connectionEventsMutex);
    handle->connectionEventsQueue.push(std::move(evPtr));
    return NULL;

};

void* udpClientEventHandler(std::unique_ptr<Event> evPtr, std::optional<sf::IpAddress> &remoteAddress, unsigned short &remotePort, void *args){
    std::cout << "got udp event\n";
    struct clientEventHandlerData *handle = (clientEventHandlerData*) args;
    std::lock_guard<std::mutex> queueLockGuard(handle->connectionEventsMutex);
    handle->connectionEventsQueue.push(std::move(evPtr));
    return NULL;
}

Client::Client() : conn(ClientConnection::createClientConnection({127, 0, 0, 1}, 42069)){
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
                    tickToDisplay = 0;
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
        }
    }
    sf::sleep(sf::milliseconds(20));
}

void Client::run(){
    gameStates.push(ClientGameState());
    //create a fake(e.g. default) stage, has to be reworked
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
        gameStates[latestGeneratedTick].gameState.setStage(s2);
        baseGameState.setStage(s2);
    }
    
    //enter the main loop
    mainLoop();
}

void Client::processEventsAwaitingSpawn(){

    std::lock_guard<std::mutex> queueLockGuard(eventData.connectionEventsMutex);
    while(!eventData.connectionEventsQueue.empty()){
//        std::cout<< "processEvents: processing a new event\n";
        auto& evPtr = eventData.connectionEventsQueue.front();
        Event* ev = evPtr.get();
        //somehow handle tha event
        EventSpawnNewPlayer *evSpawnNewPlayer = dynamic_cast<EventSpawnNewPlayer*>(ev);
        if(evSpawnNewPlayer != NULL){
            gameStates[latestGeneratedTick].gameState.addPlayer(Player(evSpawnNewPlayer->playerId, sf::Vector2f(40.f, 40.f),evSpawnNewPlayer->location));
            if(evSpawnNewPlayer->playerId == this->playerId){
                // we have spawned and can now start the game
                clientState = PLAYING;
                eventData.connectionEventsQueue.pop();
                return;
            }
        }
        eventData.connectionEventsQueue.pop();
    }
}

void Client::processEventsPlaying(){

    std::lock_guard<std::mutex> queueLockGuard(eventData.connectionEventsMutex);
    while(!eventData.connectionEventsQueue.empty()){
//        std::cout<< "processEvents: processing a new event\n";
        auto& evPtr = eventData.connectionEventsQueue.front();
        Event* ev = evPtr.get();
        EventGamestatePlayerInputHistory *eventGamestatePlayerInputHistory = dynamic_cast<EventGamestatePlayerInputHistory*>(ev);
        if(eventGamestatePlayerInputHistory != NULL){
            std::cout << "handling gameStateUpdate\n";
            updateGameStates(*eventGamestatePlayerInputHistory);
        }
        
        // EventPlayerVelocity *evPlayerVelocity = dynamic_cast<EventPlayerVelocity*>(ev);
        // if(evPlayerVelocity != NULL){
        //     updatePlayerVelocity(gameState, evPlayerVelocity->playerId,evPlayerVelocity->velocity);
        // }
/*        EventPlayerLocation *evPlayerLocation = dynamic_cast<EventPlayerLocation*>(ev);
        if(evPlayerLocation != NULL){
            updatePlayerLocation(gameState, evPlayerLocation->playerId,evPlayerLocation->location);
        }*/
/*        EventUserInput *evUserInput = dynamic_cast<EventUserInput*>(ev);
        if(evUserInput != NULL && evUserInput->playerInput.playerId != this->playerId){
            std::cout << "received user input\n";
            playerInput input = evUserInput->playerInput.playerInput;
            Player& player = gameState.getPlayer(evUserInput->playerInput.playerId);
            
            // Horizontal velocity
            sf::Vector2f v = player.getVelocity();
            v.x = 0.f;
            if(input.moveLeft) { player.setFacing(-1); v.x -= player.getSpeed(); }
            if(input.moveRight) { player.setFacing( 1); v.x += player.getSpeed(); }

            // Jump
            if(input.jump && player.getIsOnGround() && playerId == player.getId()){
                v.y = -400.f;
                player.setIsOnGround(false);
            }

            // apply velocity
            player.setVelocity(v);

            // fire projectile
            if(input.projectile && player.getProjectileCooldown() == 0){
                int projId = gameState.getProjectileIds();
                gameState.setProjectileIds(projId + 1);

                Projectile proj(projId, {20.f,20.f}, player.getPosition());
                proj.setSpeed(proj.getSpeed() * player.getFacing());
                gameState.addProjectile(proj);

                player.setProjectileCooldown(100);
            }
        }*/

//        printf("processEvents: done processing event\n");
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
        std::cout << "handling update info for tick " << currentTickInfo << '\n';
        LabeledUpdateInfo update = ev.getNextInfo();

        //size == 1 -> highestIndex(0) currentTickInfo == 1:
        while(gameStates.getSize() <= currentTickInfo){
            std::cout << "pushing new gameState\n";
            gameStates.push(ClientGameState());
        }

        if(gameStates[currentTickInfo].state == ClientGameState::UNINITIALIZED){
            gameStates[currentTickInfo].playerInputs = update.info.pInput;
            gameStates[currentTickInfo].state = ClientGameState::READY_FOR_GENERATION;
        }
        if(update.type == UpdateInfo::GAMESTATE_PLAYER_INPUT && gameStates[currentTickInfo].state != ClientGameState::GENERATED){
            std::cout << "got whole update for tick " << currentTickInfo << '\n';
            
            
            std::cout << "update playerInfos size: " << update.info.gsUpdate.playerInfos.size() << '\n';
            if(latestGeneratedTick < gameStates.getMinIndex()){
                gameStates[currentTickInfo].gameState = baseGameState;
            }else{
                gameStates[currentTickInfo].gameState = gameStates[latestGeneratedTick].gameState;
            }
            update.info.gsUpdate.applyUpdate(gameStates[currentTickInfo].gameState);
            gameStates[currentTickInfo].state = ClientGameState::GENERATED;
            latestGeneratedTick = currentTickInfo;
        }
        currentTickInfo++;
    }
}

void Client::mainLoop(){
    //float deltaTime = ...
    //if deltaTime > tickrate:
    //  deltaTime -= tickrate
    //  processInputs(keypresses)
    //  processEvents(//this may build several new local GameStates)
    //  if(latestGameState().getPlayer(this->playerId)) != gameStates{acknowledgedClientInputs}.localPlayer
    //      gameStates{acknowledgedClientInputs}.localPlayer = latestGameState().getPlayer(this->playerId))
    //      for all gameStates>acknowledgedClientInputs: recalculate ghostPlayer
    //  else{updateGhostPlayer on gameStateToDisplay+1 using tickrate}
    //  updateGame(gameStateToDisplay+1, tickrate)
    //displayGameState = updateGame_interpolate(gameStateToDisplay, deltaTime)
    //render(displayGameState)
    printf("entering main loop\n");
    //this is the main loop
    sf::Clock tickClock;
    sf::Time tickRate = sf::milliseconds(tickrateMs);
    
    while(true){
        if(clientState == PLAYING){
            sf::Time deltaTime = tickClock.restart();
            printf("be playing\n");

            processEventsPlaying();
            processInputs();
/*            ClientGameStateUpdater updater(gameStates[latestRenderedTick].gameState);
            std::vector<playerInputWithId> playerInputs;
            updateGame(updater, playerInputs, gameStates[latestRenderedTick].gameState, deltaTime);*/

            bool canRenderTick = true;
            while (deltaTime >= tickRate){
                deltaTime-= tickRate;
                tickToDisplay++;
                std::cout << "client display info(tickToDisplay, latestGeneratedTick, gameStatesSize): " << tickToDisplay << " " << latestGeneratedTick << " " << gameStates.getSize() << '\n';
                if (gameStates.getSize() <= tickToDisplay){
                    std::cout << "nothing to generate yet, backing out\n";
                    canRenderTick = false;
                    continue;
                }
                //find baseGameState tick to generate from
                bool foundTickToUse = false;
                TICK_TYPE generatedTickToUse = 0;
                for (TICK_TYPE t = tickToDisplay; t >= gameStates.getMinIndex(); t--)
                {
                    if(gameStates[t].state == ClientGameState::GENERATED){
                        generatedTickToUse = t;
                        std::cout << "foundTickToUs\n";
                        foundTickToUse = true;
                        break;
                    }else if(gameStates[t].state == ClientGameState::UNINITIALIZED){
                        //were missing playerInputs and cannot generate
                        std::cout << "foundUninitializedTick at "<< t << '\n';
                        canRenderTick = false;
                        break;
                    }
                }
                if(!foundTickToUse){
                    //well were fucked, nothing to display as we have no known state
                    canRenderTick = false;
                    continue;
                }
                //generate all ticks from baseTick till we have the current one
                for (TICK_TYPE t = generatedTickToUse + 1; t <= tickToDisplay; t++)
                {
                    gameStates[t].gameState = gameStates[t-1].gameState;
                    ClientGameStateUpdater gsUpdater(gameStates[t].gameState);
                    updateGame(gsUpdater, gameStates[t].playerInputs, gameStates[t].gameState, tickrateMs);
                }
                canRenderTick = true;
            }
            if(canRenderTick){
                std::cout << "rendering tick "<< tickToDisplay << '\n';
                renderer.render(gameStates[tickToDisplay].gameState);
                renderer.processDisplayEvents();
            }else{
                std::cout << "can't render tick:(\n";
            }
            if(tickClock.getElapsedTime().asMilliseconds() >= 5){
                std::cout << "Computing tick took " << tickClock.getElapsedTime().asMilliseconds() << "ms\n";
            }

            sf::sleep(tickRate - tickClock.getElapsedTime());
        }
        else if(clientState == AWAITING_SPAWN){
            printf("be awaiting spawn\n");
            processEventsAwaitingSpawn();
            sf::sleep(sf::milliseconds(10));
        }
    }
    printf("exiting main loop\n");
}

playerInput Client::processInputs(){
    playerInput input;
    if(clientState == PLAYING){
        playerInputWithId playerInputWithId = {0}; //player inputs that are sent to the server
        playerInputWithId.playerId = playerId;
//        sf::Vector2f playerVelocity = gameState.getPlayer(playerId).getVelocity();
//        playerVelocity.x = 0.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)){
            input.moveLeft = true;
/*            std::cout << "---User pressed A\n";
            std::cout << "current player velocity: " << playerVelocity.x << '\n';
            gameState.getPlayer(playerId).setFacing(Player::FACING_LEFT);
            playerVelocity.x -= gameState.getPlayer(playerId).getSpeed();
            std::cout << "new player velocity: " << playerVelocity.x << '\n';
            playerInputWithId.playerInput.moveLeft = true;*/
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)){
            input.moveRight = true;
/*            std::cout << "---User pressed D";
            gameState.getPlayer(playerId).setFacing(Player::FACING_RIGHT);
            playerVelocity.x += gameState.getPlayer(playerId).getSpeed();
            playerInputWithId.playerInput.moveRight = true;*/
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
            input.jump = true;
            //playerVelocity.y = -400.f;
            //gameState.getPlayer(playerId).setIsOnGround(false);
//            playerInputWithId.playerInput.jump = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::J)) {
            input.projectile = true;
/*            Player& player = gameState.getPlayer(playerId);
            if (player.getProjectileCooldown() == 0) {
                std::cout << "--Projectile fired";
                Projectile newProjectile(gameState.getProjectileIds(), sf::Vector2f(20.0, 20.0), gameState.getPlayer(playerId).getPosition());
                gameState.setProjectileIds(gameState.getProjectileIds()+1); // increment so the next bullet has new ID
                newProjectile.setSpeed(newProjectile.getSpeed() * (player.getFacing() == Player::FACING_RIGHT ? 1 : -1));
                gameState.addProjectile(newProjectile);
                player.setProjectileCooldown(100);
                playerInputWithId.playerInput.projectile = true;
            }*/
        }
/*        if(playerVelocity != gameState.getPlayer(playerId).getVelocity()){
            gameState.getPlayer(playerId).setVelocity(playerVelocity);
        }*/
        playerInputWithId.playerId = this->playerId;
        playerInputWithId.playerInput = input;
        EventUserInput ev = EventUserInput(playerInputWithId);
        conn.sendTcpEvent(ev);
        return input;
    }
    return playerInput();
}