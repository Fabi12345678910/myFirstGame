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
    void*(*eventHandler)(const Event&, const Connection&) = NULL;
    pthread_t eventHandlerThread;
    std::unique_ptr<Event> receiveNextEvent();
    Connection(std::unique_ptr<sf::TcpSocket> ptr){
        this->socket = std::move(ptr);
    };
    ~Connection() = default;
    void sendEvent(const Event& ev);
    void setEventHandler(void* handleEvent(const Event&, const Connection&));
};

#endif