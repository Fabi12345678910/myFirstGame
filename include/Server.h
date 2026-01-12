#pragma once
#include "GameState.h"
#include "Networking/Event.h"
#include "Inputs.h"
#include <queue>
#include <atomic>
#include <mutex>
#include <tuple>
#include <unordered_map>
#include <Networking/ServerConnection.h>
#include <Networking/ServerSocket.h>
#include "CircularArray.h"

#ifndef ENABLE_SERVER_RENDERING
    #define ENABLE_SERVER_RENDERING false
#endif
#if ENABLE_SERVER_RENDERING
    #include "Renderer.h"
#endif

struct serverEventHandlerData{
    std::queue<std::tuple<ServerConnection&, std::unique_ptr<Event>>> connectionEventsQueue;
    std::mutex connectionEventsMutex;
    std::list<std::unique_ptr<ServerConnection>>* connections;
};
struct selectedMapsByClients{
    std::unordered_map<OBJECT_ID_TYPE, int16_t> byPlayer;
    std::vector<OBJECT_ID_TYPE> arrivalOrder;
};

class Server
{
    
private:

    std::atomic<bool> ready = false; // helps client know when to join
    static constexpr int MAX_PLAYERS = 4;
    static constexpr int MAX_GAMEOBJECTS = 10000;
    //Tickrate in milliseconds per ticks
    static constexpr std::uint16_t TICKRATE_MS = 10;
    static constexpr sf::Time tickRate = sf::milliseconds(TICKRATE_MS);
    static const int gameStateBufferSize = 64;
    //defines how many gameState ticks will be sent
    //e.g. 1 = send every frame
    //e.g. 8 = send full update every 8th frame
    static const int gameStateResyncTicks = 2;
    TICK_TYPE currentTick = 0;

    int numPlayers = 0;
    CircularArray<std::vector<struct indexedPlayerInputWithId>, gameStateBufferSize> inputHistory;
    CircularArray<GameState, gameStateBufferSize> gameStates;
    void processEvents(std::vector<indexedPlayerInputWithId>& playerInputs);
    void someTimesResyncGameState();
    void resyncGameState();
    void resyncLastInputs();
    void mainLoop();
    void sendServerHealth();
    std::queue<OBJECT_ID_TYPE> availablePlayerIds;
    std::queue<OBJECT_ID_TYPE> availableObjectIds;
    struct serverEventHandlerData eventData;
    ServerSocket serverSocket;

    // should maybe be placed in game state
    selectedMapsByClients selectedMaps;

    std::unordered_map<OBJECT_ID_TYPE, TICK_TYPE> respawnAtTick;

    #if ENABLE_SERVER_RENDERING
    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode(sf::Vector2u(1280, 720)), "ServerRendering");
    Renderer renderer = Renderer(window);
    #endif
public:
    void run();
    bool isReady() const { return ready.load(); }
    Server();
    Server(unsigned short port);
    ~Server();
};
