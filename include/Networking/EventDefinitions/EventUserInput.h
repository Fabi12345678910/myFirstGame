#pragma once

#include "Networking/Event.h"
#include "Networking/EventTypeList.h"
#include "Inputs.h"
#include "Types.h"


class EventUserInput:public Event
{
private:
    std::vector<playerInput> playerInputs;
    TICK_TYPE firstInputTick;
    TICK_TYPE currentReadInputTick;
public:
    EventUserInput(sf::Packet packet){
        uint32_t inputSize;
        if(!(packet >> firstInputTick >> inputSize)){
            throw std::runtime_error("error reading packet");
        };
        currentReadInputTick = firstInputTick;
        playerInputs.reserve(inputSize);
        for (uint32_t i = 0; i < inputSize; i++)
        {
            playerInputs.emplace_back();
            if(!(packet >> playerInputs.back())){
                throw std::runtime_error("error reading playerInputs");
            };
        }
    };

    EventUserInput(){
    }

    bool hasNextUserInput(){
        return (currentReadInputTick - firstInputTick < (TICK_TYPE)playerInputs.size());
    }

    indexedPlayerInput getNextUserInput(){
        indexedPlayerInput retValue;
        retValue.playerInput = playerInputs[currentReadInputTick - firstInputTick ];
        retValue.idx = currentReadInputTick;
        currentReadInputTick++;
        return retValue;
    }

    void addUserInput(TICK_TYPE tick, playerInput playerInput){
        if(playerInputs.size() == 0){
            firstInputTick = tick;
        }
        if(firstInputTick + playerInputs.size() != tick){
            throw std::runtime_error("provided userInputs are not in order");
        }
        playerInputs.push_back(playerInput);
    }
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_USER_INPUT;
        packet << firstInputTick;
        packet << (uint32_t) playerInputs.size();
        for(auto& input: playerInputs){
            packet << input;
        }
        return packet;
    }
};
