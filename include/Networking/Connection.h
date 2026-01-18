#pragma once
#include "Networking/Event.h"
#include "Threads/Threads.h"
#include <SFML/Network.hpp>
#include <atomic>
#include <memory>


class Connection
{
private:
    std::unique_ptr<sf::TcpSocket> tcpSocket;
    std::atomic_bool connectionDead = false;
public:
    void*(*eventHandler)(std::unique_ptr<Event>, Connection&, void* args) = NULL;
    void* eventHandlerArgs;

    pthread_t eventHandlerThread;
    std::unique_ptr<Event> receiveNextEvent();
    //additional arguments passed into the event handler
    Connection(std::unique_ptr<sf::TcpSocket> tcpSocket, void* eventHandlerArgs = NULL)
    : eventHandlerArgs(eventHandlerArgs), tcpSocket(std::move(tcpSocket)){};

    ~Connection() = default;

    virtual void setArgs(void* args){
        this->eventHandlerArgs = args;
    }
    void* getArgs(){
        return this->eventHandlerArgs;
    };
    void sendTcpEvent(const Event& ev);
    void setEventHandler(void* handleEvent(std::unique_ptr<Event>, Connection&, void* args));
};
