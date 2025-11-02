#include "Server.h"
#include "GameUpdate.h"

#include "../src/networking/EventDefinitions/EventLoginRequest.hpp"
#include "../src/networking/EventDefinitions/EventLoginConfirmation.hpp"
#include "../src/networking/EventDefinitions/EventLoginDenied.hpp"
#include "../src/networking/EventDefinitions/EventDebugMessage.hpp"
#include "../src/networking/EventDefinitions/EventSpawnNewPlayer.hpp"
#include "../src/networking/EventDefinitions/EventPlayerVelocity.hpp"

#define MAX_PLAYERS 4
#define MAX_GAMEOBJECTS 10000
//Tickrate in milliseconds per ticks
#define TICKRATE_MS 10

void* eventHandler(std::unique_ptr<Event> ev, Connection& conn, void* args) {
    struct eventHandlerData *handle = (eventHandlerData*) args;
    std::lock_guard<std::mutex> queueLockGuard(handle->connectionEventsMutex);
    ServerConnection* serverConn = dynamic_cast<ServerConnection*>(&conn);
    if(serverConn == NULL){
        throw std::runtime_error("did not get a server connection");
    }
    std::tuple<ServerConnection&, std::unique_ptr<Event>> queueEntry(*serverConn, std::move(ev));
    handle->connectionEventsQueue.push(std::move(queueEntry));
    printf("handling event\n");
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
    serverSocket.setArgs(&eventData);
    serverSocket.setEventHandler(eventHandler);
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
    //this is the main loop
    sf::Clock tickClock;
    while(true){
        float deltaTime = tickClock.restart().asSeconds();
        processEvents();
        updateGamestate(deltaTime);
        int32_t sleep_ms = TICKRATE_MS - tickClock.getElapsedTime().asMilliseconds();
        if(tickClock.getElapsedTime().asMilliseconds() >= 1){
            std::cout << "Computing tick took " << tickClock.getElapsedTime().asMilliseconds() << "ms\n";
        }
        sf::sleep(sf::milliseconds(TICKRATE_MS) - tickClock.getElapsedTime());
    }
    printf("exiting main loop\n");
}

void Server::processEvents(){
    std::lock_guard<std::mutex> queueLockGuard(eventData.connectionEventsMutex);
    while(!eventData.connectionEventsQueue.empty()){
        std::cout<< "processEvents: processing a new event\n";
        auto& connEv = eventData.connectionEventsQueue.front();
        ServerConnection& conn = std::get<0>(connEv);
        Event* ev = std::get<1>(connEv).get();
        //somehow handle tha event
        EventLoginRequest *evLoginRequest = dynamic_cast<EventLoginRequest*>(ev);
        if(evLoginRequest != NULL){
            std::cout << "got a new login request\n";
            if(availablePlayerIds.empty()){
                //no new SLOT
                conn.sendEvent(EventLoginDenied(0));
            }else{

                OBJECT_ID_TYPE nextPlayerId = availablePlayerIds.front();
                availablePlayerIds.pop();
                conn.setPlayerId(nextPlayerId);
                conn.sendEvent(EventLoginConfirmation(nextPlayerId));
                
                gameState.addPlayer(Player(nextPlayerId, sf::Vector2f(40.f, 40.f), sf::Vector2f(400.f, 10.f)));
                serverSocket.sendEventToEveryone(EventSpawnNewPlayer(gameState.getPlayer(nextPlayerId).getPosition(), nextPlayerId));
            }
        }

        EventPlayerVelocity *evVelocity = dynamic_cast<EventPlayerVelocity*>(ev);
        if(evVelocity != NULL){
            //TODO check velocity bounds
        }

        EventDebugMessage *evDebug = dynamic_cast<EventDebugMessage*>(ev);
        if(evDebug != NULL){
            std::cout << "got a debug message\n";
            std::cout << "Debug message: " << evDebug->getMessage() << '\n';
        }

        printf("processEvents: done processing event\n");
        eventData.connectionEventsQueue.pop();
    }
}

void Server::updateGamestate(float deltaTime){
    updateGame(gameState, deltaTime);
}

int main(int argc, char const *argv[])
{
    Server server;
    server.run();
    return 0;
}
