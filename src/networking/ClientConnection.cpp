#include "Networking/ClientConnection.h"
#include "Networking/Events.h"
#include "plog/Log.h"

void *clientUdpListener(void* arg){
    ClientConnection *conn = (ClientConnection*) arg;
    sf::Packet packet;
    std::optional<sf::IpAddress> remoteAdress;
    unsigned short remotePort;
    while (true)
    {
        if(conn->udpSocket.receive(packet, remoteAdress, remotePort) != sf::Socket::Status::Done){
            PLOG_ERROR << "error reading udp packet";
        }

        conn->udpEventHandler(getEventFromPacket(packet), remoteAdress, remotePort, conn->udpArgs);
    }
}

void ClientConnection::setUdpEventHandler(void* udpEventHandler(std::unique_ptr<Event>, std::optional<sf::IpAddress>& remoteAddress, unsigned short& remotePort, void* args)){
    if(this->udpEventHandler != NULL){
        throw std::runtime_error("eventHandler already set");
    }
    this->udpEventHandler = udpEventHandler;

    pthread_create(&eventHandlerThread, NULL, clientUdpListener, this);
}