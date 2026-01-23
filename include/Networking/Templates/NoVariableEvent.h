#pragma once
#include "Networking/Event.h"

template<DATATYPE_EVENT_TYPE eventId>
class NoVariableEvent: public Event{
    public:
    NoVariableEvent(sf::Packet packet){
    };
    NoVariableEvent(){
    }
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) eventId;
        return packet;
    }
};