#pragma once

#include "Networking/Event.h"
#include "Networking/EventTypeList.h"
#include "Types.h"
#include <SFML/System/Vector2.hpp>

class EventPlayerLocation:public Event
{
public:
    OBJECT_ID_TYPE playerId;
    sf::Vector2f location;
    EventPlayerLocation(sf::Packet packet){
        if(!(packet >> playerId)){
            throw std::runtime_error("failed to read playerId");
        }
        if(!(packet >> location.x)){
            throw std::runtime_error("failed to read x location");
        };
        if(!(packet >> location.y)){
            throw std::runtime_error("failed to read y location");
        };
    };
    EventPlayerLocation(OBJECT_ID_TYPE playerId, sf::Vector2f location) : playerId(playerId), location(location){
    }
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_PLAYER_LOCATION;
        packet << playerId;
        packet << location.x;
        packet << location.y;
        return packet;
    }
};
