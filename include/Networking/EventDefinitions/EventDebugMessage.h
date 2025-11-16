#pragma once

#include "Networking/Event.h"
#include "Networking/EventTypeList.h"

class EventDebugMessage:public Event
{
public:
    std::string message;
    EventDebugMessage(sf::Packet packet){
        if(!(packet >> message)){
            throw std::runtime_error("failed to read debug message");
        };
    };
    EventDebugMessage(const std::string& msg){
        this->message = msg;
    }
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_DEBUG_MESSAGE;
        packet << message;
        return packet;
    }
};
