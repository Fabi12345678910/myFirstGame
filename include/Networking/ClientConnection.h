#pragma once

#include "Networking/Connection.h"

class ClientConnection : public Connection
{
private:

public:
    ClientConnection(std::unique_ptr<sf::TcpSocket> tcpSocket, std::unique_ptr<sf::UdpSocket> udpSocket, sf::IpAddress udpReceipientIpAdress, unsigned short udpRecipientPort, void* eventHandlerArgs = NULL)
        : Connection(std::move(tcpSocket), std::move(udpSocket), udpReceipientIpAdress, udpRecipientPort, eventHandlerArgs){
    }
    static ClientConnection createClientConnection(sf::IpAddress targetIpAdress, unsigned short recipientPort, void* eventHandlerArgs = NULL){
        std::unique_ptr<sf::TcpSocket> tcpSocket = std::make_unique<sf::TcpSocket>();
        if (tcpSocket->connect(targetIpAdress, recipientPort) != sf::Socket::Status::Done)
        {
            throw std::runtime_error("Error connecting to tcp server");
        }
        std::unique_ptr<sf::UdpSocket> udpSocket = std::make_unique<sf::UdpSocket>();
        if(udpSocket->bind(sf::Socket::AnyPort) != sf::Socket::Status::Done){
            throw std::runtime_error("Error binding local udp port");
        }
        return ClientConnection(std::move(tcpSocket), std::move(udpSocket), targetIpAdress, recipientPort, eventHandlerArgs);
    }
};
