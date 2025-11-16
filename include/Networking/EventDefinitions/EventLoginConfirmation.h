#pragma once

#include "Networking/Event.h"
#include "Networking/EventTypeList.h"
#include "Types.h"

class EventLoginConfirmation:public Event
{
public:
    OBJECT_ID_TYPE playerId;
    EventLoginConfirmation(sf::Packet packet){
        if(!(packet >> playerId)){
            throw std::runtime_error("failed to read player Id");
        };
    };
    EventLoginConfirmation(OBJECT_ID_TYPE playerId){
        this->playerId = playerId;
    }
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_LOGIN_CONFIRMATION;
        packet << playerId;
        return packet;
    }
};
