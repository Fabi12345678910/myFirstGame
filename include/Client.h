#pragma once

#include "GameState.h"
#include "Renderer.h"
#include "Networking/ClientConnection.h"
#include "Networking/Event.h"

#include <queue>
#include <mutex>
struct clientEventHandlerData{
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
    void mainLoop();
    void processInputs();
    GameState gameState = GameState();
    struct clientEventHandlerData eventData;
    ClientConnection conn;
public:
    void run();
    Client();
    ~Client(){};
};
