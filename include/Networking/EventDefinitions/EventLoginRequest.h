#pragma once
#include "Networking/Event.h"
#include "Networking/EventTypeList.h"

class EventLoginRequest:public Event
{
public:
    unsigned short udpPort;
    EventLoginRequest(sf::Packet packet){
        if(!(packet >> udpPort)){
            throw std::runtime_error("failed to read udp port");
        };
    };
    EventLoginRequest(unsigned short udpPort): udpPort(udpPort){
    }
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_LOGIN_REQUEST;
        packet << udpPort;
        return packet;
    }
};