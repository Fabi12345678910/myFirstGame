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
    EventLoginConfirmation(sf::Packet packet){
        if(!(packet >> playerId >> latestServerTick >> serverTickRateMs)){
            throw std::runtime_error("failed to event contents");
        };
    };
    EventLoginConfirmation(OBJECT_ID_TYPE playerId, TICK_TYPE latestServerTick, std::uint16_t serverTickRateMs):
        playerId(playerId), latestServerTick(latestServerTick), serverTickRateMs(serverTickRateMs){} 

    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_LOGIN_CONFIRMATION;
        packet << playerId;
        packet << latestServerTick;
        packet << serverTickRateMs;
        return packet;
    }
};
