#pragma once

#include "../Event.hpp"
#include "../EventTypeList.hpp"
#include <SFML/System/Vector2.hpp>
#include "Types.h"

class EventSpawnNewPlayer:public Event
{
private:
    OBJECT_ID_TYPE playerId;
    sf::Vector2f location;
public:
    EventSpawnNewPlayer(sf::Packet packet){
        if(!(packet >> playerId)){
            throw std::runtime_error("failed to new player id");
        }
        if(!(packet >> location.x)){
            throw std::runtime_error("failed to read x location");
        };
        if(!(packet >> location.y)){
            throw std::runtime_error("failed to read y location");
        };
    };
    EventSpawnNewPlayer(sf::Vector2f location, OBJECT_ID_TYPE playerId) : location(location), playerId(playerId){
    }
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_SPAWN_NEW_PLAYER;
        packet << playerId;
        packet << location.x;
        packet << location.y;
        return packet;
    }
    sf::Vector2f getLocation(){
        return location;
    }
    void setLocation(sf::Vector2f location){
        this->location = location;
    }

    OBJECT_ID_TYPE getPlayerId(){
        return playerId;
    }

    void setPlayerId(OBJECT_ID_TYPE playerId){
        this->playerId = playerId;
    }
};
