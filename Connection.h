#ifndef _CONNECTION_H
#define _CONNECTION_H

#include "Event.h"
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include "Events.h"

class Connection
{
private:
    int connectionSocket;
    pthread_t eventHandlerThread;
public:
    Connection(int socket):connectionSocket(socket){

    };
    ~Connection();
    Event& receiveNextEvent();
    void* (*eventHandler)(Event&) = NULL;
    int sendEvent(Event& event);
    int setEventHandler(void* handleEvent(Event&));
};
#endif