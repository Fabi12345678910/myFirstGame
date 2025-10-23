#ifndef _SERVER_SOCKET_HPP
#define _SERVER_SOCKET_HPP

#include "ServerConnection.hpp"
#include <SFML/Network/TcpListener.hpp>
#include <list>

class ServerSocket
{
private:
    unsigned short port;
public:
    sf::TcpListener listener;
    std::list<std::unique_ptr<ServerConnection>> connections;
    void*(*eventHandler)(const Event&, const Connection&) = NULL;
    pthread_t connectionHandlerThread;

    ServerSocket(unsigned short listenerPort);
    ~ServerSocket() = default;
    //not implemented yet, unsure if ever happens
    void sendEventToPlayer(int id, Event& ev) = delete;
    void sendEventToEveryone(Event &ev);
    void setEventHandler(void* handleEvent(const Event&, const Connection&));
};


#endif