#include "ServerConnection.h"
#include <stdio.h>
#include <iostream>

ServerConnection::~ServerConnection()
{
    fprintf(stderr, "destructing ServerConnection with socket %d\n", connectionSocket);
    if(eventHandler != NULL){
        pthread_cancel(eventHandlerThread);
    }
    close(connectionSocket);
}

int ServerConnection::sendEvent(Event& event){
    std::size_t remainingBytes = event.getDataSize();
    void* sendingPtr = event.getData();
    while (remainingBytes > 0)
    {
        ssize_t sentBytes = send(connectionSocket, sendingPtr, remainingBytes, 0);
        if(sentBytes <= 0){
            return -1;
        }
        remainingBytes -= sentBytes;
    }
    return 0;
}

Event& ServerConnection::receiveNextEvent(){

    struct event nextEvent;
    ssize_t read_size = recv(connectionSocket, &nextEvent, sizeof(nextEvent), 0);
    if (read_size == 0){
        throw std::runtime_error("connection closed");
    }
    return createEventFromEventData(nextEvent);
}

static void* eventHandlerFunction(void* arg){
    ServerConnection* conn = (ServerConnection*) arg;
    while(true){
        try
        {
            Event& ev = conn->receiveNextEvent();
            conn->eventHandler(ev);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            delete conn;
            return NULL;
        }
    }
}

int ServerConnection::setEventHandler(void* handleEvent(Event&)){
    if(this->eventHandler != NULL){
        return 1;
    }
    this->eventHandler = handleEvent;
    pthread_create(&eventHandlerThread, NULL, eventHandlerFunction, this);
    return 0;
}
