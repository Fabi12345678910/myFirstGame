#pragma once
#include "GameState.h"
#include "../src/networking/ClientConnection.hpp"
#include "../src/networking/Event.hpp"

#include <queue>
#include <mutex>
struct eventHandlerData{
    std::queue<std::tuple<ClientConnection&, std::unique_ptr<Event>>> connectionEventsQueue;
    std::mutex connectionEventsMutex;
};

class Client
{
private:
    sf::RenderWindow window;
    int playerId;
    void performLogin();
    void processEvents();
    void updateGamestate(float deltaTime);
    void mainLoop();
    GameState gameState = GameState();
    struct eventHandlerData eventData;
    ClientConnection conn;
public:
    void run();
    Client(/* args */);
    ~Client(){};
};
