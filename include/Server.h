#pragma once
#include "GameState.h"
#include "../src/networking/Event.hpp"
#include <queue>
#include <mutex>
#include <tuple>
#include <../src/networking/ServerConnection.hpp>
#include <../src/networking/ServerSocket.hpp>

#ifndef ENABLE_SERVER_RENDERING
    #define ENABLE_SERVER_RENDERING true
#endif
#if ENABLE_SERVER_RENDERING
    #include "Renderer.h"
#endif

struct eventHandlerData{
    std::queue<std::tuple<ServerConnection&, std::unique_ptr<Event>>> connectionEventsQueue;
    std::mutex connectionEventsMutex;
};

class Server
{
private:
    void processEvents();
    void updateGamestate(float deltaTime);
    void mainLoop();
    GameState gameState = GameState();
    std::queue<OBJECT_ID_TYPE> availablePlayerIds;
    std::queue<OBJECT_ID_TYPE> availableObjectIds;
    struct eventHandlerData eventData;
    ServerSocket serverSocket;

    #if ENABLE_SERVER_RENDERING
    Renderer renderer;
    #endif
public:
    void run();
    Server();
    ~Server();
};
