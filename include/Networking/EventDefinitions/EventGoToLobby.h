#pragma once

#include "Networking/Event.h"
#include "Networking/EventTypeList.h"
#include "Types.h"
#include <SFML/Network.hpp>
#include <stdexcept>

class EventGoToLobby : public Event
{
public:
    EventGoToLobby(sf::Packet packet) {
    }

    explicit EventGoToLobby(){};

    sf::Packet toPacket() const override {
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE)EVENT_TYPE_GO_TO_LOBBY;
        return packet;
    }
};