#ifndef _SERVER_CONNECTION_HPP
#define _SERVER_CONNECTION_HPP

#include "Connection.hpp"
#include "Types.h"

class ServerConnection : public Connection
{
private:
    OBJECT_ID_TYPE playerId = 0;
public:
    ServerConnection(std::unique_ptr<sf::TcpSocket> ptr, void* args = NULL)
        : Connection(std::move(ptr), args) {}
    OBJECT_ID_TYPE getPlayerId(){
        return playerId;
    }
    void setPlayerId(OBJECT_ID_TYPE id){
        playerId = id;
    }
};

#endif