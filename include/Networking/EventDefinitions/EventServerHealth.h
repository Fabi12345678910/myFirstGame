#pragma once

#include "Networking/Event.h"
#include "Networking/EventTypeList.h"

#define HEALTH_INPUT_QUEUE_TYPE std::uint8_t
#define HEALTH_FRAME_TIME_TYPE std::uint8_t

class EventServerHealth:public Event
{
public:
    HEALTH_INPUT_QUEUE_TYPE inputsInQueue;
    HEALTH_FRAME_TIME_TYPE frameTimeMs;
    EventServerHealth(sf::Packet packet){
        if(!(packet >> inputsInQueue >> frameTimeMs)){
            throw std::runtime_error("error reading packet");
        };
    };

    EventServerHealth(HEALTH_INPUT_QUEUE_TYPE inputsInQueue, HEALTH_FRAME_TIME_TYPE frameTimeMs)
        :inputsInQueue(inputsInQueue), frameTimeMs(frameTimeMs){
    }

    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_SERVER_HEALTH;
        packet << inputsInQueue;
        packet << frameTimeMs;
        return packet;
    }
};
