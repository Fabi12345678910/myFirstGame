#pragma once
#include "Networking/Event.h"

template<DATATYPE_EVENT_TYPE eventId, typename T>
class SingleVariableEvent: public Event{
    public:
    T item;
    SingleVariableEvent(sf::Packet packet){
        if(!(packet >> item)){
            throw std::runtime_error("failed to read packet");
        };
    };
    SingleVariableEvent(const T& item) : item(item){
    }
    SingleVariableEvent(const T&& item) : item(item){
    }

    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) eventId;
        packet << item;
        return packet;
    }
};