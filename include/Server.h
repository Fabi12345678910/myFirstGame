#pragma once
#include "GameState.h"
#include "Networking/Event.h"
#include <queue>
#include <mutex>
#include <tuple>
#include <Networking/ServerConnection.h>
#include <Networking/ServerSocket.h>

#ifndef ENABLE_SERVER_RENDERING
    #define ENABLE_SERVER_RENDERING true
#endif
#if ENABLE_SERVER_RENDERING
    #include "Renderer.h"
#endif

struct serverEventHandlerData{
    std::queue<std::tuple<ServerConnection&, std::unique_ptr<Event>>> connectionEventsQueue;
    std::mutex connectionEventsMutex;
};

class Server
{
private:
    void processEvents();
    void updateGamestate(float deltaTime);
    void someTimesResyncPlayers();
    void resyncPlayers();
    void mainLoop();
    GameState gameState = GameState();
    std::queue<OBJECT_ID_TYPE> availablePlayerIds;
    std::queue<OBJECT_ID_TYPE> availableObjectIds;
    struct serverEventHandlerData eventData;
    ServerSocket serverSocket;

    #if ENABLE_SERVER_RENDERING
    Renderer renderer;
    #endif
public:
    void run();
    Server();
    ~Server();
};
