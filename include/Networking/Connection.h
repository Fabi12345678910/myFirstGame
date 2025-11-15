#pragma once
#include "Networking/Event.h"
#include <sys/socket.h>
#include <unistd.h>
#include <SFML/Network.hpp>
#include <memory>

class Connection
{
private:
    std::unique_ptr<sf::UdpSocket> udpSocket;
    sf::IpAddress udpRecipientAdress;
    unsigned short udpRecipientPort;
    std::unique_ptr<sf::TcpSocket> tcpSocket;
    void* eventHandlerArgs;
public:
    void*(*eventHandler)(std::unique_ptr<Event>, Connection&, void* args) = NULL;
    unsigned short getUdpPort(){return udpSocket->getLocalPort();}
    pthread_t eventHandlerThread;
    std::unique_ptr<Event> receiveNextEvent();
    //additional arguments passed into the event handler
    Connection(std::unique_ptr<sf::TcpSocket> tcpSocket, std::unique_ptr<sf::UdpSocket> udpSocket, sf::IpAddress udpReceipientIpAdress, unsigned short udpRecipientPort, void* eventHandlerArgs = NULL)
    : udpRecipientAdress(udpReceipientIpAdress)
    ,udpRecipientPort(udpRecipientPort)
    ,eventHandlerArgs(eventHandlerArgs){
        this->tcpSocket = std::move(tcpSocket);
        this->udpSocket = std::move(udpSocket);
    };
    ~Connection() = default;

    virtual void setArgs(void* args){
        this->eventHandlerArgs = args;
    }
    void* getArgs(){
        return this->eventHandlerArgs;
    };
    void sendEvent(const Event& ev);
    void setEventHandler(void* handleEvent(std::unique_ptr<Event>, Connection&, void* args));
};
