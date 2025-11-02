#pragma once

#include "../Event.hpp"
#include "../EventTypeList.hpp"
#include "Types.h"

class EventLoginConfirmation:public Event
{
private:
    OBJECT_ID_TYPE playerId;
public:
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
    OBJECT_ID_TYPE getPlayerId(){
        return playerId;
    }
    void setPlayerId(OBJECT_ID_TYPE id){
        this->playerId = id;
    }
};
