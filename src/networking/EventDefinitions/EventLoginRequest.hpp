#pragma once
#include "../Event.hpp"
#include "../EventTypeList.hpp"

class EventLoginRequest:public Event
{
public:
    EventLoginRequest(sf::Packet packet){
    };
    EventLoginRequest(){
    }
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_LOGIN_REQUEST;
        return packet;
    }
};