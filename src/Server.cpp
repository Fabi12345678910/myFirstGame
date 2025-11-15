#include "Server.h"
#include "GameUpdate.h"
#include "PlayerOperations.h"

#include "Networking/EventDefinitions/EventLoginRequest.h"
#include "Networking/EventDefinitions/EventLoginConfirmation.h"
#include "Networking/EventDefinitions/EventLoginDenied.h"
#include "Networking/EventDefinitions/EventDebugMessage.h"
#include "Networking/EventDefinitions/EventSpawnNewPlayer.h"
#include "Networking/EventDefinitions/EventPlayerVelocity.h"
#include "Networking/EventDefinitions/EventPlayerLocation.h"
#include "maps/Map_TestAll.h"

#define MAX_PLAYERS 4
#define MAX_GAMEOBJECTS 10000
//Tickrate in milliseconds per ticks
#define TICKRATE_MS 10

void* serverEventHandler(std::unique_ptr<Event> ev, Connection& conn, void* args) {
    struct serverEventHandlerData *handle = (serverEventHandlerData*) args;
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
    //this is the main loop
    sf::Clock tickClock;
    while(true){
        float deltaTime = tickClock.restart().asSeconds();
        processEvents();
        updateGamestate(deltaTime);
        someTimesResyncPlayers();
        int32_t sleep_ms = TICKRATE_MS - tickClock.getElapsedTime().asMilliseconds();
        if(tickClock.getElapsedTime().asMilliseconds() >= 1){
            std::cout << "Computing tick took " << tickClock.getElapsedTime().asMilliseconds() << "ms\n";
        }
        #if ENABLE_SERVER_RENDERING
        renderer.render(gameState);
        renderer.processDisplayEvents();
        #endif
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
                conn.sendTcpEvent(EventLoginDenied(0));
            }else{

                OBJECT_ID_TYPE nextPlayerId = availablePlayerIds.front();
                availablePlayerIds.pop();
                conn.setPlayerId(nextPlayerId);
                conn.sendTcpEvent(EventLoginConfirmation(nextPlayerId));
                
                //send all players to current player for now, should later be included in a gamestate sync
                for(Player& p : gameState.getPlayers()){
                    conn.sendTcpEvent(EventSpawnNewPlayer(p.getPosition(), p.getId()));
                }
                gameState.addPlayer(Player(nextPlayerId, sf::Vector2f(40.f, 40.f), sf::Vector2f(400.f, 10.f)));
                
                serverSocket.sendEventToEveryone(EventSpawnNewPlayer(gameState.getPlayer(nextPlayerId).getPosition(), nextPlayerId));
            }
        }

        EventPlayerVelocity *evVelocity = dynamic_cast<EventPlayerVelocity*>(ev);
        if(evVelocity != NULL){
            std::cout << "received velocity update: " << evVelocity->getVelocity().x << ',' << evVelocity->getVelocity().y << '\n';
            updatePlayerVelocity(gameState, conn.getPlayerId(), evVelocity->getVelocity());
            serverSocket.sendEventToEveryone(EventPlayerVelocity(evVelocity->getPlayerId(), evVelocity->getVelocity()));
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

void Server::someTimesResyncPlayers(){
    #define PLAYERRESYNCTIMER 4
    static unsigned tickCounter = PLAYERRESYNCTIMER;
    tickCounter--;
    if(tickCounter == 0){
        tickCounter = PLAYERRESYNCTIMER;
        resyncPlayers();
    }
}

void Server::resyncPlayers(){
    std::cout << "resyncing players\n";
    for(Player &p : gameState.getPlayers()){
        serverSocket.sendEventToEveryone(EventPlayerLocation(p.getId(), p.getPosition()));
    }
}
