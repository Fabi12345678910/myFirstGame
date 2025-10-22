#ifndef _CLIENT_CONNECTION_H
#define _CLIENT_CONNECTION_H

#include "Event.h"
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include "Events.h"

class ClientConnection
{
private:
    int connectionSocket;
    pthread_t eventHandlerThread;
public:
    ClientConnection(int socket):connectionSocket(socket){

    };
    ~ClientConnection();
    Event& receiveNextEvent();
    void* (*eventHandler)(Event&) = NULL;
    int sendEvent(Event& event);
    int setEventHandler(void* handleEvent(Event&));
};
#endif