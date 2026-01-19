#include "Networking/Connection.h"
#include "Networking/Events.h"
#include "plog/Log.h"
#include <SFML/Network.hpp>
#include <SFML/Network/Socket.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>

std::unique_ptr<Event> Connection::receiveNextEvent(){
    sf::Packet pack;
    while (true) {
        if(cancelEventHandler.load()){
            throw std::runtime_error("thread getting cancelled");
        }
        auto result = tcpSocket->receive(pack);
        if(result == sf::Socket::Status::Done){
            return getEventFromPacket(pack);}
        else if(result ==sf::Socket::Status::NotReady){
            sf::sleep(sf::milliseconds(1));}
        else{
            throw std::runtime_error("Error receiving Packet");
        }
    }
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
            PLOG_ERROR << e.what() << '\n';
            conn->connectionDead.store(true);
            return NULL;
        }
    }
}

void Connection::setEventHandler(void* handleEvent(std::unique_ptr<Event>, Connection&, void* args)){
    if(this->eventHandler != NULL){
        throw std::runtime_error("eventHandler already set");
    }
    this->eventHandler = handleEvent;
    tcpSocket->setBlocking(false);
    if(!pthread_create(&eventHandlerThread, NULL, eventHandlerFunction, this)){
        this->eventHandlerThreadRunning = true;
    }else{
        PLOG_ERROR << "error starting eventHandlerThread";
    }
}

void Connection::sendTcpEvent(const Event& ev){
    sf::Packet pack = ev.toPacket();
    if(tcpSocket->send(pack) != sf::Socket::Status::Done){
        connectionDead.store(true);
        throw std::runtime_error("error sending event");
    }
}