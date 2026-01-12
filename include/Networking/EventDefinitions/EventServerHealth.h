#pragma once

#include "Networking/Event.h"
#include "Networking/EventTypeList.h"

class EventServerHealth:public Event
{
public:
    std::uint8_t inputsInQueue;
    EventServerHealth(sf::Packet packet){
        if(!(packet >> inputsInQueue)){
            throw std::runtime_error("error reading packet");
        };
    };

    EventServerHealth(std::uint8_t inputsInQueue) : inputsInQueue(inputsInQueue){
    }

    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_SERVER_HEALTH;
        packet << inputsInQueue;
        return packet;
    }
};
