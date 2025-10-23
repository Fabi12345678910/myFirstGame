#include "../ServerSocket.hpp"

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

        serverSock->connections.push_back(std::make_unique<ServerConnection>(std::move(sock)));
        if (serverSock->eventHandler != NULL){
            serverSock->connections.back()->setEventHandler(serverSock->eventHandler);
        }
        printf("created new client\n");
    }    
}

ServerSocket::ServerSocket(unsigned short listenerPort){
    this->port = listenerPort;
    if (listener.listen(this->port) != sf::Socket::Status::Done)
    {
        throw std::runtime_error("error listening on port");
    }
    pthread_create(&connectionHandlerThread, NULL, connectionAccepter, this);
}

void ServerSocket::setEventHandler(void* handleEvent(const Event&, const Connection&)){
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

void ServerSocket::sendEventToEveryone(Event &ev){
    for (std::unique_ptr<ServerConnection>& connection : connections){
        if(connection->eventHandler!= NULL){
            connection->sendEvent(ev);
        }
    }
}