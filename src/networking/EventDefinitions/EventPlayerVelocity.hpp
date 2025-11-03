#pragma once

#include "../Event.hpp"
#include "../EventTypeList.hpp"
#include <SFML/System/Vector2.hpp>

class EventPlayerVelocity:public Event
{
private:
    OBJECT_ID_TYPE playerId;
    sf::Vector2f velocity;
public:
    EventPlayerVelocity(sf::Packet packet){
        if(!(packet >> playerId)){
            throw std::runtime_error("failed to read playerId");
        }
        if(!(packet >> velocity.x)){
            throw std::runtime_error("failed to read x velocity");
        };
        if(!(packet >> velocity.y)){
            throw std::runtime_error("failed to read y velocity");
        };
    };
    EventPlayerVelocity(OBJECT_ID_TYPE playerId, sf::Vector2f velocity) : playerId(playerId), velocity(velocity){
    }
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_PLAYER_VELOCITY;
        packet << playerId;
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
    
    OBJECT_ID_TYPE getPlayerId(){
        return playerId;
    }

    void setPlayerId(OBJECT_ID_TYPE playerId){
        this->playerId = playerId;
    }
};
