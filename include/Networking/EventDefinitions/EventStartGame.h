#pragma once

#include "Networking/Event.h"
#include "Networking/EventTypeList.h"
#include "Types.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Network.hpp>
#include <cstdint>
#include <vector>

inline sf::Packet& operator <<(sf::Packet& packet, const std::vector<std::uint8_t>& vec)
{
    packet << static_cast<std::uint32_t>(vec.size());
    for (std::uint8_t v : vec) {
        packet << v;
    }
    return packet;
}

inline sf::Packet& operator >>(sf::Packet& packet, std::vector<std::uint8_t>& vec)
{
    std::uint32_t count = 0;
    packet >> count;
    vec.resize(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        packet >> vec[i];
    }
    return packet;
}

class EventStartGame:public Event
{
public:
    TICK_TYPE gameStartTick;
    int16_t stageId;
    std::vector<std::uint8_t> spawnPoints;

    EventStartGame(sf::Packet packet){
        if(!(packet >> gameStartTick)){
            throw std::runtime_error("failed to read gameStartTick");
        };
        if(!(packet >> stageId)){
            throw std::runtime_error("failed to read stageId");
        };
        if(!(packet >> spawnPoints)){
            throw std::runtime_error("failed to read spawnPoints");
        };
    };
    EventStartGame(TICK_TYPE gameStartTick, int16_t stageId, std::vector<std::uint8_t> spawnPoints)
        : gameStartTick(gameStartTick), stageId(stageId), spawnPoints(std::move(spawnPoints)){}
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_START_GAME;
        packet << gameStartTick;
        packet << stageId;
        packet << spawnPoints;
        return packet;
    }

    


};
