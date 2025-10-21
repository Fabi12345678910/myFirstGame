#include "Connection.h"
#include <stdio.h>

Connection::~Connection()
{
    fprintf(stderr, "destruction Connection with socket %d\n", connectionSocket);
    if(eventHandler != NULL){
        pthread_cancel(eventHandlerThread);
    }
    close(connectionSocket);
}

int Connection::sendEvent(Event& event){
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

Event& Connection::receiveNextEvent(){

    struct event nextEvent;
    recv(connectionSocket, &nextEvent, sizeof(nextEvent), 0);
    return createEventFromEventData(nextEvent);
}

void* eventHandlerFunction(void* arg){
    Connection* conn = (Connection*) arg;
    while(true){
        Event& ev = conn->receiveNextEvent();
        conn->eventHandler(ev);
    }
}

int Connection::setEventHandler(void* handleEvent(Event&)){
    if(this->eventHandler != NULL){
        return 1;
    }
    this->eventHandler = handleEvent;
    pthread_create(&eventHandlerThread, NULL, eventHandlerFunction, this);
    return 0;
}