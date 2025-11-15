#pragma once
#include "Networking/Event.h"
#include <sys/socket.h>
#include <unistd.h>
#include <SFML/Network.hpp>
#include <memory>

class Connection
{
private:
    std::unique_ptr<sf::UdpSocket> udpSocketOwned;
    sf::UdpSocket* udpSocket;
    sf::IpAddress udpRecipientAdress;
    unsigned short udpRecipientPort;
    std::unique_ptr<sf::TcpSocket> tcpSocket;
public:
    void*(*eventHandler)(std::unique_ptr<Event>, Connection&, void* args) = NULL;
    void* eventHandlerArgs;
    unsigned short getUdpPort(){return udpSocket->getLocalPort();}
    pthread_t eventHandlerThread;
    std::unique_ptr<Event> receiveNextEvent();
    //additional arguments passed into the event handler
    Connection(std::unique_ptr<sf::TcpSocket> tcpSocket, sf::UdpSocket& udpSocket, sf::IpAddress udpReceipientIpAdress, unsigned short udpRecipientPort, void* eventHandlerArgs = NULL)
    : udpRecipientAdress(udpReceipientIpAdress)
    ,udpRecipientPort(udpRecipientPort)
    ,eventHandlerArgs(eventHandlerArgs)
    ,udpSocket(&udpSocket){
        this->tcpSocket = std::move(tcpSocket);
    };

    Connection(std::unique_ptr<sf::TcpSocket> tcpSocket, std::unique_ptr<sf::UdpSocket> udpSocket, sf::IpAddress udpReceipientIpAdress, unsigned short udpRecipientPort, void* eventHandlerArgs = NULL)
    : udpRecipientAdress(udpReceipientIpAdress)
    ,udpRecipientPort(udpRecipientPort)
    ,eventHandlerArgs(eventHandlerArgs){
        this->tcpSocket = std::move(tcpSocket);
        this->udpSocketOwned = std::move(udpSocket);
        this->udpSocket = this->udpSocketOwned.get();
    };
    ~Connection() = default;

    virtual void setArgs(void* args){
        this->eventHandlerArgs = args;
    }
    void* getArgs(){
        return this->eventHandlerArgs;
    };
    void sendTcpEvent(const Event& ev);
    void setEventHandler(void* handleEvent(std::unique_ptr<Event>, Connection&, void* args));
    sf::IpAddress getUdpRecipientAdress(){return udpRecipientAdress;};
    unsigned short getUdpRecipientPort(){return udpRecipientPort;};
//    void setUdpRecipientAdress(sf::IpAddress udpRecipientAdress){this->udpRecipientAdress = udpRecipientAdress;}
    void setUdpRecipientPort(unsigned short udpRecipientPort){this->udpRecipientPort = udpRecipientPort;}
};
