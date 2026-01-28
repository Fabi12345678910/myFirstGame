#pragma once

#include "Networking/Event.h"
#include "Networking/EventTypeList.h"
#include "Types.h"

class EventLoginConfirmation:public Event
{
public:
    OBJECT_ID_TYPE playerId;
    TICK_TYPE latestServerTick;
    std::uint16_t serverTickRateMs;
    std::int16_t currentMap;
    EventLoginConfirmation(sf::Packet packet){
        if(!(packet >> playerId >> latestServerTick >> serverTickRateMs >> currentMap)){
            throw std::runtime_error("failed to read event contents");
        };
    };
    EventLoginConfirmation(OBJECT_ID_TYPE playerId, TICK_TYPE latestServerTick, std::uint16_t serverTickRateMs, std::int16_t currentMap):
        playerId(playerId), latestServerTick(latestServerTick), serverTickRateMs(serverTickRateMs), currentMap(currentMap){} 

    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_LOGIN_CONFIRMATION;
        packet << playerId;
        packet << latestServerTick;
        packet << serverTickRateMs;
        packet << currentMap;
        return packet;
    }
};
