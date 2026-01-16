#pragma once

#include "Networking/Event.h"
#include "Networking/EventTypeList.h"
#include <SFML/System/Vector2.hpp>

class EventSelectedMap:public Event
{
public:
    int16_t stageId;
    EventSelectedMap(sf::Packet packet){
        if(!(packet >> stageId)){
            throw std::runtime_error("failed to read stageId");
        };
    };
    EventSelectedMap(int16_t stageId): stageId(stageId){}
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_SELECTED_MAP;
        packet << stageId;
        return packet;
    }
};
