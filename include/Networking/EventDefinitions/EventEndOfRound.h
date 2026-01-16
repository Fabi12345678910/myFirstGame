#pragma once

#include "Networking/Event.h"
#include "Networking/EventTypeList.h"
#include "Types.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Network.hpp>
#include <cstdint>
#include <vector>

class EventEndOfRound:public Event
{
public:
    OBJECT_ID_TYPE winningPlayerId;

    EventEndOfRound(sf::Packet packet){
        if(!(packet >> winningPlayerId)){
            throw std::runtime_error("failed to read winningPlayerId");
        };
    };
    EventEndOfRound(OBJECT_ID_TYPE playerId)
        : gameStartTick(gameStartTick), stageId(stageId), spawnPoints(std::move(spawnPoints)){}
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_END_OF_ROUND;
        packet << winningPlayerId;
        return packet;
    }

    


};
