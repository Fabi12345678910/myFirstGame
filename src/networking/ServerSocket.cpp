#include "Networking/ServerSocket.h"
#include "Networking/Events.h"

void *connectionAccepter(void * arg){
    ServerSocket *serverSock = (ServerSocket*) arg;
    std::unique_ptr<sf::TcpSocket> sock;
    while (true)
    {
        sock = std::make_unique<sf::TcpSocket>();
        if(serverSock->listener.accept(*sock) != sf::Socket::Status::Done){
            std::cerr << "error accepting new client";
            continue;
        }

        serverSock->connections.push_back(std::make_unique<ServerConnection>(std::move(sock), serverSock->udpSocket , sock->getRemoteAddress().value(), 0, serverSock->args));
        if (serverSock->eventHandler != NULL){
            serverSock->connections.back()->setEventHandler(serverSock->eventHandler);
        }
        printf("created new client\n");
    }    
}

void *udpListener(void* arg){
    ServerSocket *serverSock = (ServerSocket*) arg;
    sf::Packet packet;
    std::optional<sf::IpAddress> remoteAdress;
    unsigned short remotePort;
    while (true)
    {
        if(serverSock->udpSocket.receive(packet, remoteAdress, remotePort) != sf::Socket::Status::Done){
            std::cerr << "error reading udp packet";
        }

        serverSock->udpEventHandler(getEventFromPacket(packet), remoteAdress, remotePort, serverSock->udpArgs);
    }
}

ServerSocket::ServerSocket(unsigned short listenerPort){
    this->port = listenerPort;
    if (listener.listen(this->port) != sf::Socket::Status::Done){
        throw std::runtime_error("error listening on tcp port");
    }
    if(this->udpSocket.bind(listenerPort) != sf::Socket::Status::Done){
        throw std::runtime_error("error listening on udp port");
    }
    pthread_create(&connectionHandlerThread, NULL, connectionAccepter, this);
}

void ServerSocket::setEventHandler(void* handleEvent(std::unique_ptr<Event>, Connection&, void* args)){
    if (this->eventHandler == NULL){
        this->eventHandler = handleEvent;
        for (std::unique_ptr<ServerConnection>& connection : connections){
            if(connection->eventHandler!= NULL){
                connection->setEventHandler(handleEvent);
            }
        }
    }else{
        throw std::runtime_error("eventHandler already set");
    }
}

void ServerSocket::setUdpEventHandler(void* udpEventHandler(std::unique_ptr<Event>, std::optional<sf::IpAddress>& remoteAddress, unsigned short& remotePort, void* args)){
    if (this->udpEventHandler == NULL){
        this->udpEventHandler = udpEventHandler;

        pthread_create(&udpEventHandlerThread, NULL, udpListener, this);
    }else{
        throw std::runtime_error("udpEventHandler already set");
    }
}

void ServerSocket::setArgs(void* args){
    this->args = args;
    for (std::unique_ptr<ServerConnection>& connection : connections){
        connection->setArgs(args);
    }
}

void ServerSocket::sendEventToEveryone(Event &&ev){
    for (std::unique_ptr<ServerConnection>& connection : connections){
        if(connection->eventHandler!= NULL){
            connection->sendTcpEvent(ev);
        }
    }
}