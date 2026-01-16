#pragma once

#include "Config.h"
#include "Networking/Connection.h"
#include "plog/Log.h"

class ClientConnection : public Connection
{
private:
    sf::IpAddress udpRecipientAdress;
    unsigned short udpRecipientPort;

public:
    sf::UdpSocket udpSocket;
    void*(*udpEventHandler)(std::unique_ptr<Event>, std::optional<sf::IpAddress>& remoteAddress, unsigned short& remotePort, void* args) = NULL;
    pthread_t udpEventHandlerThread;
    void* udpArgs = NULL;

    ClientConnection(std::unique_ptr<sf::TcpSocket> tcpSocket, sf::IpAddress udpRecipientAdress, unsigned short udpRecipientPort, void* eventHandlerArgs = NULL)
        : Connection(std::move(tcpSocket), eventHandlerArgs), udpRecipientAdress(udpRecipientAdress), udpRecipientPort(udpRecipientPort){
        
        if(udpSocket.bind(sf::Socket::AnyPort) != sf::Socket::Status::Done){
            throw std::runtime_error("Error binding local udp port");
        }
    }

    static ClientConnection createClientConnection(sf::IpAddress targetIpAdress, unsigned short recipientPort, void* eventHandlerArgs = NULL){
        std::unique_ptr<sf::TcpSocket> tcpSocket = std::make_unique<sf::TcpSocket>();
        if (tcpSocket->connect(targetIpAdress, recipientPort) != sf::Socket::Status::Done)
        {
            throw std::runtime_error("Error connecting to tcp server");
        }
        return ClientConnection(std::move(tcpSocket), targetIpAdress, recipientPort, eventHandlerArgs);
    }
    unsigned short getUdpPort(){return udpSocket.getLocalPort();}
    void sendUdpEvent(const Event& ev){
        sf::Packet pack = ev.toPacket();
        PLOG_DEBUG_IF(debugClientNetworking) << "sending user input to: "<< udpRecipientAdress <<':' << udpRecipientPort;
        if(udpSocket.send(pack, udpRecipientAdress, udpRecipientPort) != sf::Socket::Status::Done){
            throw std::runtime_error("error sending event");
        }
    }
    sf::IpAddress getUdpRecipientAdress(){return udpRecipientAdress;}
    unsigned short getUdpRecipientPort(){return udpRecipientPort;}
    void setUdpRecipientPort(unsigned short udpRecipientPort){this->udpRecipientPort = udpRecipientPort;}
    void setUdpArgs(void* args){this->udpArgs = args;}
    void* getUdpArgs(){return this->udpArgs;}
    void setUdpEventHandler(void* udpEventHandler(std::unique_ptr<Event>, std::optional<sf::IpAddress>& remoteAddress, unsigned short& remotePort, void* args));
};
