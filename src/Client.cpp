#include "Client.h"
#include "PlayerOperations.h"
#include "Projectile.h"
#include "GameUpdate.h"
#include "Renderer.h"

#include "Networking/EventDefinitions/EventLoginRequest.h"
#include "Networking/EventDefinitions/EventLoginDenied.h"
#include "Networking/EventDefinitions/EventLoginConfirmation.h"
#include "Networking/EventDefinitions/EventPlayerLocation.h"
#include "Networking/EventDefinitions/EventPlayerVelocity.h"
#include "Networking/EventDefinitions/EventSpawnNewPlayer.h"

#include "maps/Map_TestAll.h"

Client::Client() : conn(ClientConnection::createClientConnection({127, 0, 0, 1}, 42069)){
    
    performLogin();
}

void* clientEventHandler(std::unique_ptr<Event> ev, Connection& conn, void* args) {
    struct clientEventHandlerData *handle = (clientEventHandlerData*) args;
    std::lock_guard<std::mutex> queueLockGuard(handle->connectionEventsMutex);
    ClientConnection* clientConn = dynamic_cast<ClientConnection*>(&conn);
    if(clientConn == NULL){
        throw std::runtime_error("did not get a server connection");
    }
    std::tuple<ClientConnection&, std::unique_ptr<Event>> queueEntry(*clientConn, std::move(ev));
    handle->connectionEventsQueue.push(std::move(queueEntry));
    printf("handling event\n");
    return NULL;
};

void Client::performLogin(){
    
    conn.sendTcpEvent(EventLoginRequest(conn.getUdpPort()));
    conn.setArgs(&eventData);
    conn.setEventHandler(clientEventHandler);
    while (true)
    {
        sf::sleep(sf::milliseconds(20));
        std::lock_guard<std::mutex> queueLockGuard(eventData.connectionEventsMutex);
        while(!eventData.connectionEventsQueue.empty()){
            auto& connEv = eventData.connectionEventsQueue.front();
            ClientConnection& conn = std::get<0>(connEv);
            Event* ev = std::get<1>(connEv).get();
            EventLoginConfirmation* evLoginSuccess = dynamic_cast<EventLoginConfirmation*>(ev);
            if(evLoginSuccess != NULL){
                this->playerId = evLoginSuccess->playerId;
                return;
            }
            EventLoginDenied*evLoginDenied  = dynamic_cast<EventLoginDenied*>(ev);
            if(evLoginDenied != NULL){
                throw std::runtime_error("Login denied");
            }
        }
    }
}

void Client::run(){
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
        gameState.setStage(s2);
    }
    
    //enter the main loop
    mainLoop();
}

void Client::processEvents(){

    std::lock_guard<std::mutex> queueLockGuard(eventData.connectionEventsMutex);
    while(!eventData.connectionEventsQueue.empty()){
        std::cout<< "processEvents: processing a new event\n";
        auto& connEv = eventData.connectionEventsQueue.front();
        ClientConnection& conn = std::get<0>(connEv);
        Event* ev = std::get<1>(connEv).get();
        //somehow handle tha event
        EventSpawnNewPlayer *evSpawnNewPlayer = dynamic_cast<EventSpawnNewPlayer*>(ev);
        if(evSpawnNewPlayer != NULL){
            if(evSpawnNewPlayer->playerId == this->playerId){
                // we have spawned and can now start the game
                clientState = PLAYING;

            }
            gameState.addPlayer(Player(evSpawnNewPlayer->playerId, sf::Vector2f(40.f, 40.f),evSpawnNewPlayer->location));
        }
        EventPlayerVelocity *evPlayerVelocity = dynamic_cast<EventPlayerVelocity*>(ev);
        if(evPlayerVelocity != NULL){
            updatePlayerVelocity(gameState, evPlayerVelocity->playerId,evPlayerVelocity->velocity);
        }
        EventPlayerLocation *evPlayerLocation = dynamic_cast<EventPlayerLocation*>(ev);
        if(evPlayerLocation != NULL){
            updatePlayerLocation(gameState, evPlayerLocation->playerId,evPlayerLocation->location);
        }
        eventData.connectionEventsQueue.pop();
    }
}
void Client::updateGamestate(float deltaTime){
    updateGame(gameState, deltaTime);
}
void Client::mainLoop(){
    printf("entering main loop\n");
    //this is the main loop
    sf::Clock tickClock;
    while(true){
        float deltaTime = tickClock.restart().asSeconds();
        processEvents();
        processInputs();
        updateGamestate(deltaTime);
        renderer.render(gameState);
        renderer.processDisplayEvents();
        if(tickClock.getElapsedTime().asMilliseconds() >= 1){
            std::cout << "Computing tick took " << tickClock.getElapsedTime().asMilliseconds() << "ms\n";
        }
        sf::sleep(sf::milliseconds(10) - tickClock.getElapsedTime());
    }
    printf("exiting main loop\n");
}

void Client::processInputs(){
    if(clientState == PLAYING){
        sf::Vector2f playerVelocity = gameState.getPlayer(playerId).getVelocity();
        playerVelocity.x = 0.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)){
            std::cout << "---User pressed A\n";
            std::cout << "current player velocity: " << playerVelocity.x << '\n';
            gameState.getPlayer(playerId).setFacing(-1);
            playerVelocity.x -= gameState.getPlayer(playerId).getSpeed();
            std::cout << "new player velocity: " << playerVelocity.x << '\n';
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)){
            std::cout << "---User pressed D";
            gameState.getPlayer(playerId).setFacing(1);
            playerVelocity.x += gameState.getPlayer(playerId).getSpeed();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && gameState.getPlayer(playerId).getIsOnGround()) {
            playerVelocity.y = -400.f;
            gameState.getPlayer(playerId).setIsOnGround(false);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::J)) {
            Player& player = gameState.getPlayer(playerId);
            if (player.getCooldown() == 0) {
                std::cout << "--Projectile fired";
                Projectile newProjectile(gameState.getBulletIDs(), sf::Vector2f(20.0, 20.0), gameState.getPlayer(playerId).getPosition());
                gameState.setBulletIDs(gameState.getBulletIDs()+1); // increment so the next bullet has new ID
                newProjectile.setSpeed(newProjectile.getSpeed() * player.getFacing());
                gameState.addProjectile(newProjectile);
                player.setCooldown(100);
            }
        }
        if(playerVelocity != gameState.getPlayer(playerId).getVelocity()){
            std::cout << "player has speed" << gameState.getPlayer(playerId).getSpeed() << '\n';
            std::cout << "setting player velocity to" << playerVelocity.x << ',' << playerVelocity.y << '\n';
            gameState.getPlayer(playerId).setVelocity(playerVelocity);
            std::cout << "players gameState Velocity" << gameState.getPlayer(playerId).getVelocity().x << ',' << gameState.getPlayer(playerId).getVelocity().y << '\n';
            conn.sendTcpEvent(EventPlayerVelocity(playerId, playerVelocity));

        }
    }
}