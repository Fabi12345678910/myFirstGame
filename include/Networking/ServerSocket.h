#pragma once

#include "Networking/ServerConnection.h"
#include <SFML/Network/TcpListener.hpp>
#include <list>

class ServerSocket
{
private:
    unsigned short port;
public:
    sf::TcpListener listener;
    sf::UdpSocket udpSocket;
    std::list<std::unique_ptr<ServerConnection>> connections;

    void*(*eventHandler)(std::unique_ptr<Event>, Connection&, void* args) = NULL;
    void*(*udpEventHandler)(std::unique_ptr<Event>, std::optional<sf::IpAddress>& remoteAddress, unsigned short& remotePort, void* args) = NULL;

    pthread_t connectionHandlerThread;
    pthread_t udpEventHandlerThread;

    void* args = NULL;
    void* udpArgs = NULL;

    ServerSocket(unsigned short listenerPort);
    ~ServerSocket() = default;
    //not implemented yet, unsure if ever happens
    void sendEventToPlayer(int id, Event& ev) = delete;
    void sendTcpEventToEveryone(Event&& ev);
    void sendUdpEventToEveryone(Event&& ev);
    void setEventHandler(void* handleEvent(std::unique_ptr<Event>, Connection&, void* args));
    void setUdpEventHandler(void* udpEventHandler(std::unique_ptr<Event>, std::optional<sf::IpAddress>& remoteAddress, unsigned short& remotePort, void* args));

    void setArgs(void* args);
    void* getArgs(){
        return this->args;
    };

    void setUdpArgs(void* udpArgs){
        this->udpArgs = udpArgs;
    }
    void* getUdpArgs(){
        return this->udpArgs;
    };
};
