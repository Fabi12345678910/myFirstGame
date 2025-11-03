#ifndef _CLIENT_CONNECTION_HPP
#define _CLIENT_CONNECTION_HPP

#include "Connection.hpp"

class ClientConnection : public Connection
{
private:
    /* data */
public:
    ClientConnection(sf::IpAddress target, void* args = NULL): Connection(std::make_unique<sf::TcpSocket>(), args){

        sf::Socket::Status status = socket->connect(target, 42069);
        if (status != sf::Socket::Status::Done)
        {
            throw std::runtime_error("Error connecting to server");
        }
    }
};

#endif