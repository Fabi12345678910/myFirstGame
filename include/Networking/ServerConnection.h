#pragma once

#include "Networking/Connection.h"
#include "Types.h"

class ServerConnection : public Connection
{
private:
    OBJECT_ID_TYPE playerId = 0;
public:
    ServerConnection(std::unique_ptr<sf::TcpSocket> tcpSocket, sf::UdpSocket& udpSocket, sf::IpAddress udpReceipientIpAdress, unsigned short udpRecipientPort, void* eventHandlerArgs = NULL)
        : Connection(std::move(tcpSocket), udpSocket, udpReceipientIpAdress, udpRecipientPort, eventHandlerArgs){
    }
    OBJECT_ID_TYPE getPlayerId(){
        return playerId;
    }
    void setPlayerId(OBJECT_ID_TYPE id){
        playerId = id;
    }
};
