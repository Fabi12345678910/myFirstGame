#include "Networking/Connection.h"
#include "Networking/Events.h"
#include <SFML/Network.hpp>

std::unique_ptr<Event> Connection::receiveNextEvent(){
    sf::Packet pack;
//  TODO: handle disconnect
//    if(tcpSocket->receive(pack) == sf::Socket::Status::Disconnected
    if(tcpSocket->receive(pack) != sf::Socket::Status::Done){
        throw std::runtime_error("Error receiving Packet");
    }
    return getEventFromPacket(pack);
}

static void* eventHandlerFunction(void* arg){
    Connection* conn = (Connection*) arg;
    while(true){
        try
        {
            std::unique_ptr<Event> ev = conn->receiveNextEvent();
            conn->eventHandler(std::move(ev), *conn, conn->eventHandlerArgs);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return NULL;
        }
    }
}

void Connection::setEventHandler(void* handleEvent(std::unique_ptr<Event>, Connection&, void* args)){
    if(this->eventHandler != NULL){
        throw std::runtime_error("eventHandler already set");
    }
    this->eventHandler = handleEvent;

    pthread_create(&eventHandlerThread, NULL, eventHandlerFunction, this);
}

void Connection::sendTcpEvent(const Event& ev){
    sf::Packet pack = ev.toPacket();
    if(tcpSocket->send(pack) != sf::Socket::Status::Done){
        throw std::runtime_error("error sending event");
    }
}
