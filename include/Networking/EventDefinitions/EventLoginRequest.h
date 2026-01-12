#pragma once
#include "Networking/Event.h"
#include "Networking/EventTypeList.h"
#include "Config.h"

class EventLoginRequest:public Event
{
public:
    unsigned short udpPort;
    VERSION_TYPE apiVersion;
    EventLoginRequest(sf::Packet packet){
        if(!(packet >> apiVersion >> udpPort)){
            throw std::runtime_error("failed to read packet contents");
        };
    };
    EventLoginRequest(unsigned short udpPort, VERSION_TYPE apiVersion = CONF_API_VERSION): udpPort(udpPort), apiVersion(apiVersion){
    }
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_LOGIN_REQUEST;
        packet << apiVersion;
        packet << udpPort;
        return packet;
    }
};