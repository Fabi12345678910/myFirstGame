#ifndef _SERVER_CONNECTION_HPP
#define _SERVER_CONNECTION_HPP

#include "Connection.hpp"

class ServerConnection : public Connection
{
private:
    /* data */
public:
    ServerConnection(std::unique_ptr<sf::TcpSocket> ptr)
        : Connection(std::move(ptr)) {}
};

#endif