#ifndef _SERVER_SOCKET_H
#define _SERVER_SOCKET_H

#include "Event.h"
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include "Events.h"

class ServerSocket
{
private:
    int socket;
    pthread_t connectionHandlerThread;
public:
    ServerSocket(int socketFd):socket(socketFd){

    };
    ~ServerSocket();
    int receiveNextConnection();
    void* (*connectionHandler)(int) = NULL;
    int setConnectionHandler(void* handleConnection(int socket));
};
#endif