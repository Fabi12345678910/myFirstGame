#pragma once

#include "Networking/Event.h"
#include "Networking/EventTypeList.h"
#include "Types.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Network.hpp>
#include <cstdint>
#include <stdexcept>
#include <vector>

class EventEndOfRound:public Event
{
public:
    struct PlayerScoreLine {
        OBJECT_ID_TYPE playerId;
        unsigned short score;
    };

    std::vector<PlayerScoreLine> scores;

    friend sf::Packet& operator<<(sf::Packet& packet, const PlayerScoreLine& line) {
        packet << line.playerId;
        packet << line.score;
        return packet;
    }
    friend sf::Packet& operator>>(sf::Packet& packet, PlayerScoreLine& line) {
        packet >> line.playerId;
        packet >> line.score;
        return packet;
    }

    EventEndOfRound(sf::Packet packet){
        std::uint32_t count = 0;
        if (!(packet >> count)) {
            throw std::runtime_error("failed to read scores count");
        }

        scores.clear();
        scores.reserve(static_cast<std::size_t>(count));
        for (std::uint32_t i = 0; i < count; ++i) {
            PlayerScoreLine line{};
            if (!(packet >> line)) {
                throw std::runtime_error("failed to read score line");
            }
            scores.push_back(line);
        }
    };
    explicit EventEndOfRound(std::vector<PlayerScoreLine> scores)
        : scores(std::move(scores)){}
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_END_OF_ROUND;

        packet << static_cast<std::uint32_t>(scores.size());
        for (const auto& line : scores) {
            packet << line;
        }
        return packet;
    }

};
