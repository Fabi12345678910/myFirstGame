#pragma once
#include "Networking/ServerConnection.h"
#include "Networking/ServerSocket.h"
#include "Login/PersistentServerDataProvider.h"
#include <mutex>
#include <queue>

struct loginServerEventHandlerData{
    std::queue<std::tuple<ServerConnection&, std::unique_ptr<Event>>> connectionEventsQueue;
    std::mutex connectionEventsMutex;
};

class LoginServer{
    struct loginServerEventHandlerData eventData;
    PersistentServerDataProvider& serverDataProvider;
    ServerSocket serverSocket;
    void processEvents();
    void mainLoop();
public:
    LoginServer(PersistentServerDataProvider& inputData, unsigned int port) : serverSocket(port), serverDataProvider(inputData){};
    void run();
};
