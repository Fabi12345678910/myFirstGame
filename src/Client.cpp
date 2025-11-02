#include "Client.h"
#include "PlayerOperations.h"
#include "GameUpdate.h"

#include "networking/EventDefinitions/EventLoginRequest.hpp"
#include "networking/EventDefinitions/EventLoginDenied.hpp"
#include "networking/EventDefinitions/EventLoginConfirmation.hpp"
#include "networking/EventDefinitions/EventPlayerLocation.hpp"
#include "networking/EventDefinitions/EventPlayerVelocity.hpp"
#include "networking/EventDefinitions/EventSpawnNewPlayer.hpp"

Client::Client() : conn({127, 0, 0, 1}){
    performLogin();
}

//TODO move to rendering engine
void render(sf::RenderWindow& window, GameState& gameState) {
    window.clear(sf::Color::Yellow);
    for(Player& player:gameState.getPlayers()){
        window.draw(player.getShape());
    }
    for (StageObject const& stageObject : gameState.getStage().getStageObjects()) {
        window.draw(stageObject.getShape());
    }

    window.display();
}

void* eventHandler(std::unique_ptr<Event> ev, Connection& conn, void* args) {
    struct eventHandlerData *handle = (eventHandlerData*) args;
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
    
    conn.sendEvent(EventLoginRequest());
    conn.setArgs(&eventData);
    conn.setEventHandler(eventHandler);
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
                this->playerId = evLoginSuccess->getPlayerId();
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
        Stage s = Stage(stageObjects, spawnPoints);
        gameState.setStage(s);
    }

    window = sf::RenderWindow(sf::VideoMode(sf::Vector2u(800, 600)), "My Game");
    
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
            gameState.addPlayer(Player(evSpawnNewPlayer->getPlayerId(), sf::Vector2f(40.f, 40.f),evSpawnNewPlayer->getLocation()));
        }
        EventPlayerVelocity *evPlayerVelocity = dynamic_cast<EventPlayerVelocity*>(ev);
        if(evPlayerVelocity != NULL){
            updatePlayerVelocity(gameState, evPlayerVelocity->getPlayerId(),evPlayerVelocity->getVelocity());
        }
        EventPlayerLocation *evPlayerLocation = dynamic_cast<EventPlayerLocation*>(ev);
        if(evPlayerLocation != NULL){
            updatePlayerLocation(gameState, evPlayerLocation->getPlayerId(),evPlayerLocation->getLocation());
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
//TODO         processInputs();
        updateGamestate(deltaTime);
        render(window, gameState);
        if(tickClock.getElapsedTime().asMilliseconds() >= 1){
            std::cout << "Computing tick took " << tickClock.getElapsedTime().asMilliseconds() << "ms\n";
        }
    }
    printf("exiting main loop\n");
}

int main(int argc, char const *argv[])
{
    Client client;
    client.run();
    return 0;
}