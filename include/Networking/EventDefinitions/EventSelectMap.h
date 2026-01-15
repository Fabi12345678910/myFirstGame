#pragma once

#include "Networking/Event.h"
#include "Networking/EventTypeList.h"
#include "Types.h"
#include <SFML/System/Vector2.hpp>

class EventSelectMap:public Event
{
public:
    TICK_TYPE selectMapUntil;
    EventSelectMap(sf::Packet packet){
        if(!(packet >> selectMapUntil)){
            throw std::runtime_error("failed to read selectUntil");
        };
    };
    EventSelectMap(TICK_TYPE selectMapUntil): selectMapUntil(selectMapUntil){}
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_SELECT_MAP;
        packet << selectMapUntil;
        return packet;
    }
};
