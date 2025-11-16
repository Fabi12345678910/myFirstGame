#pragma once

#include "Networking/Connection.h"
#include "Types.h"

class ServerConnection : public Connection
{
private:
    OBJECT_ID_TYPE playerId = 0;
    sf::UdpSocket& udpSocket;
public:
    sf::IpAddress udpRecipientIpAdress;
    unsigned short udpRecipientPort;
    ServerConnection(std::unique_ptr<sf::TcpSocket> tcpSocket, sf::UdpSocket& udpSocket, sf::IpAddress udpRecipientIpAdress, unsigned short udpRecipientPort, void* eventHandlerArgs = NULL)
        : Connection(std::move(tcpSocket), eventHandlerArgs), udpSocket(udpSocket), udpRecipientIpAdress(udpRecipientIpAdress), udpRecipientPort(udpRecipientPort) {
    }
    OBJECT_ID_TYPE getPlayerId(){
        return playerId;
    }
    void setPlayerId(OBJECT_ID_TYPE id){
        playerId = id;
    }
    void sendUdpEvent(const Event& ev){
        sf::Packet pack = ev.toPacket();
        if(udpSocket.send(pack, udpRecipientIpAdress, udpRecipientPort) != sf::Socket::Status::Done){
            throw std::runtime_error("error sending event");
        }
    }
};
