#pragma once

#include "Networking/Event.h"
#include "Networking/EventTypeList.h"
#include "Inputs.h"
#include "GameState.h"
#include <iterator>
#include "GameStateUpdates.h"

inline sf::Packet& operator <<(
    sf::Packet& packet,
    const std::vector<std::pair<int32_t, playerInputWithId>>& vec)
{
    packet << static_cast<uint32_t>(vec.size());

    for (const auto& pair : vec)
    {
        packet << pair.first;   // int32_t
        packet << pair.second;  // playerInputWithId
    }

    return packet;
}

inline sf::Packet& operator >>(
    sf::Packet& packet,
    std::vector<std::pair<int32_t, playerInputWithId>>& vec)
{
    uint32_t count;
    packet >> count;

    vec.resize(count);

    for (uint32_t i = 0; i < count; ++i)
    {
        int32_t key;
        playerInputWithId value;

        packet >> key;
        packet >> value;

        vec[i] = {key, value};
    }

    return packet;
}

//this will send the latest snapshots and the inputs used to create those snapshots
class EventGamestatePlayerInputHistory:public Event
{
public:
    const GameState* baseLineGameState = NULL;
    std::vector<std::pair<int32_t, gsUpdateInfo>> gameStateSnapShots;
    std::vector<std::pair<int32_t, playerInputWithId>> playerInputs;

    EventGamestatePlayerInputHistory(sf::Packet packet){
        if(!(packet >> gameStateSnapShots >> playerInputs)){
            throw std::runtime_error("failed to packetContents");
        }
    };
    EventGamestatePlayerInputHistory() {}

    //sets a gameState which is to be expected by the client, will not be sent
    //if set, even the first sent gameState will only require delta Updates
    void setBaseLineGameState(const GameState* gameState){
        baseLineGameState = gameState;
    }
    void addGameState(int32_t tick, GameState& gameState){
        //for now no delta GameStates
        auto& updateInfos = gameStateSnapShots.emplace_back();
        updateInfos.first = tick;
        for (Player& p : gameState.getPlayers()){
            updateInfos.second.playerInfos.emplace_back(p);
        }
        for (Projectile &p : gameState.getProjectiles()){
            updateInfos.second.projectileInfos.emplace_back(p);
        }
    }
    void addPlayerInput(int32_t tick, playerInputWithId pInput){
        playerInputs.push_back(std::make_pair(tick, pInput));
    }

    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_GAMESTATE_PLAYERINPUT_HISTORY;
        packet << gameStateSnapShots;
        packet << playerInputs;
        return packet;
    }
    const std::vector<std::pair<int32_t, playerInputWithId>>& getPlayerInputs(){
        return playerInputs;
    }

    const std::vector<std::pair<int32_t, gsUpdateInfo>>& getGameStateSnapShots(){
        return gameStateSnapShots;
    }
};
