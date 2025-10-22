#ifndef _SERVER_CONNECTION_H
#define _SERVER_CONNECTION_H

#include "Event.h"
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include "Events.h"

class ServerConnection
{
private:
    int connectionSocket;
    pthread_t eventHandlerThread;
public:
    ServerConnection(int socket):connectionSocket(socket){

    };
    ~ServerConnection();
    Event& receiveNextEvent();
    void* (*eventHandler)(Event&) = NULL;
    int sendEvent(Event& event);
    int setEventHandler(void* handleEvent(Event&));
};
#endif