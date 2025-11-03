#ifndef _CONNECTION_HPP
#define _CONNECTION_HPP
#include "Event.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <SFML/Network.hpp>
#include <memory>

class Connection
{
protected:
    std::unique_ptr<sf::TcpSocket> socket;

public:
    void*(*eventHandler)(std::unique_ptr<Event>, Connection&, void* args) = NULL;
    pthread_t eventHandlerThread;
    std::unique_ptr<Event> receiveNextEvent();
    //additional arguments passed into the event handler
    Connection(std::unique_ptr<sf::TcpSocket> ptr, void* args = NULL){
        this->socket = std::move(ptr);
        this->args = args;
    };
    ~Connection() = default;

    void* args = NULL;
    virtual void setArgs(void* args){
        this->args = args;
    }
    void* getArgs(){
        return this->args;
    };
    void sendEvent(const Event& ev);
    void setEventHandler(void* handleEvent(std::unique_ptr<Event>, Connection&, void* args));
};

#endif