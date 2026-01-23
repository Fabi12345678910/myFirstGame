#pragma once

#include "Networking/Event.h"
#include "Networking/EventTypeList.h"
#include "Types.h"
#include <SFML/Network.hpp>
#include <stdexcept>

class EventEndOfGame : public Event
{
public:
    OBJECT_ID_TYPE winningPlayerId;

    EventEndOfGame(sf::Packet packet) {
        if (!(packet >> winningPlayerId)) {
            throw std::runtime_error("failed to read winningPlayerId");
        }
    }

    explicit EventEndOfGame(OBJECT_ID_TYPE playerId)
        : winningPlayerId(playerId) {}

    sf::Packet toPacket() const override {
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE)EVENT_TYPE_END_OF_GAME;
        packet << winningPlayerId;
        return packet;
    }
};