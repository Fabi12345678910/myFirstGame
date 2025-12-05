#pragma once

#include "Networking/Event.h"
#include "Networking/EventTypeList.h"
#include "Inputs.h"
#include "Types.h"

class EventUserInput:public Event
{
public:
    playerInputWithId playerInput;
    EventUserInput(sf::Packet packet){
        if(!(packet >> playerInput.playerId)){
            throw std::runtime_error("failed to read playerId");
        }
        if(!(packet >> playerInput.playerInput.moveLeft)){
            throw std::runtime_error("failed to read moveLeft");
        };
        if(!(packet >> playerInput.playerInput.moveRight)){
            throw std::runtime_error("failed to read moveRight");
        };
        if(!(packet >> playerInput.playerInput.jump)){
            throw std::runtime_error("failed to read jump");
        };
        if(!(packet >> playerInput.playerInput.projectile)){
            throw std::runtime_error("failed to read projectile");
        };
    };

    EventUserInput(playerInputWithId playerInput) : playerInput(playerInput) {
    }

    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_USER_INPUT;
        packet << playerInput;
        return packet;
    }
};
