#pragma once

#include "../Event.hpp"
#include "../EventTypeList.hpp"
#include <SFML/System/Vector2.hpp>

class EventPlayerLocation:public Event
{
private:
    sf::Vector2f location;
public:
    EventPlayerLocation(sf::Packet packet){
        if(!(packet >> location.x)){
            throw std::runtime_error("failed to read x location");
        };
        if(!(packet >> location.y)){
            throw std::runtime_error("failed to read y location");
        };
    };
    EventPlayerLocation(sf::Vector2f location) : location(location){
    }
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_PLAYER_LOCATION;
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
};
