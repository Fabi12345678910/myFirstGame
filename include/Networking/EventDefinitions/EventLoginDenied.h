#pragma once
#include "Networking/Event.h"
#include "Networking/EventTypeList.h"
#include "Types.h"

class EventLoginDenied:public Event
{
private:
    std::uint16_t cause;
public:
    EventLoginDenied(sf::Packet packet){
        if(!(packet >> cause)){
            throw std::runtime_error("failed to read player Id");
        };
    };
    EventLoginDenied(std::uint16_t cause){
        this->cause = cause;
    }
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_LOGIN_DENIED;
        packet << cause;
        return packet;
    }
};