#include "../Connection.hpp"
#include "../Events.hpp"
#include <SFML/Network.hpp>

std::unique_ptr<Event> Connection::receiveNextEvent(){
    sf::Packet pack;
    if(socket->receive(pack) != sf::Socket::Status::Done){
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
            conn->eventHandler(*ev, *conn);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return NULL;
        }
    }
}

void Connection::setEventHandler(void* handleEvent(const Event&, const Connection&)){
    if(this->eventHandler != NULL){
        throw std::runtime_error("eventHandler already set");
    }
    this->eventHandler = handleEvent;

    pthread_create(&eventHandlerThread, NULL, eventHandlerFunction, this);
}

void Connection::sendEvent(const Event& ev){
    sf::Packet pack = ev.toPacket();
    if(socket->send(pack) != sf::Socket::Status::Done){
        throw std::runtime_error("error sending event");
    }
}
