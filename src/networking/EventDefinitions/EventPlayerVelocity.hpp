#pragma once

#include "../Event.hpp"
#include "../EventTypeList.hpp"
#include <SFML/System/Vector2.hpp>

class EventPlayerVelocity:public Event
{
private:
    sf::Vector2f velocity;
public:
    EventPlayerVelocity(sf::Packet packet){
        if(!(packet >> velocity.x)){
            throw std::runtime_error("failed to read x velocity");
        };
        if(!(packet >> velocity.y)){
            throw std::runtime_error("failed to read y velocity");
        };
    };
    EventPlayerVelocity(sf::Vector2f velocity) : velocity(velocity){
    }
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_PLAYER_VELOCITY;
        packet << velocity.x;
        packet << velocity.y;
        return packet;
    }
    sf::Vector2f getVelocity(){
        return velocity;
    }
    void setVelocity(sf::Vector2f location){
        this->velocity = location;
    }
};
