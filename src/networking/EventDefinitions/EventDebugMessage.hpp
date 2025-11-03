#pragma once

#include "../Event.hpp"
#include "../EventTypeList.hpp"

class EventDebugMessage:public Event
{
private:
    std::string message;
public:
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
    std::string getMessage() const {
        return message;
    }
};
