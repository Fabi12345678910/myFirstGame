#pragma once
#include "GameState.h"
#include "Renderer.h"
#include "../src/networking/ClientConnection.hpp"
#include "../src/networking/Event.hpp"

#include <queue>
#include <mutex>
struct eventHandlerData{
    std::queue<std::tuple<ClientConnection&, std::unique_ptr<Event>>> connectionEventsQueue;
    std::mutex connectionEventsMutex;
};
enum clientState{CONNECTING, PLAYING};

class Client
{
private:
    enum clientState clientState = CONNECTING;
    int playerId;
    Renderer renderer;
    void performLogin();
    void processEvents();
    void updateGamestate(float deltaTime);
    void mainLoop();
    void processInputs();
    GameState gameState = GameState();
    struct eventHandlerData eventData;
    ClientConnection conn;
public:
    void run();
    Client(/* args */);
    ~Client(){};
};
