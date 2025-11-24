#pragma once
#include "GameState.h"
#include "Networking/Event.h"
#include "Inputs2.h"
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
    static const int gameStateBufferSize = 64;
    //defines how many gameState ticks will be sent
    //e.g. 1 = send every frame
    //e.g. 8 = send full update every 8th frame
    static const int gameStateResyncTicks = 4;
    uint64_t currentFrame = 0;

    int numPlayers;
    std::array<GameState, gameStateBufferSize> gameStates = {};
    void processEvents(std::vector<playerInputWithId>& playerInputs);
    void someTimesResyncGameState();
    void resyncGameState();
    void mainLoop();
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
