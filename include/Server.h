#include "GameState.h"
#include "../src/networking/Event.hpp"
#include <queue>
#include <mutex>
#include <tuple>
#include <../src/networking/ServerConnection.hpp>
#include <../src/networking/ServerSocket.hpp>

struct eventHandlerData{
    std::queue<std::tuple<ServerConnection&, std::unique_ptr<Event>>> connectionEventsQueue;
    std::mutex connectionEventsMutex;
};

class Server
{
private:
    void processEvents();
    void updateGamestate(int32_t deltaTime);
    void mainLoop();
    GameState gameState = GameState();
    std::queue<OBJECT_ID_TYPE> availablePlayerIds;
    std::queue<OBJECT_ID_TYPE> availableObjectIds;
    struct eventHandlerData eventData;
    ServerSocket serverSocket;

public:
    void run();
    Server();
    ~Server();
};
