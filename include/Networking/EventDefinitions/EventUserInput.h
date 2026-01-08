#pragma once

#include "Networking/EventDefinitions/UdpClientSendableEvent.h"
#include "Networking/EventTypeList.h"
#include "Inputs.h"
#include "Types.h"


class EventUserInput:public UdpClientSendableEvent
{
private:
    std::vector<struct indexedPlayerInput> playerInputs;
    TICK_TYPE currentReadInputTick = 0;
public:
    EventUserInput(sf::Packet packet){
        if(!(packet >> playerId)){
            throw std::runtime_error("error reading packet");
        }
        uint32_t inputSize;
        if(!(packet >> inputSize)){
            throw std::runtime_error("error reading packet");
        };
        playerInputs.reserve(inputSize);
        for (uint32_t i = 0; i < inputSize; i++)
        {
            playerInputs.emplace_back();
            if(!(packet >> playerInputs.back())){
                throw std::runtime_error("error reading playerInputs");
            };
        }
    };

    EventUserInput(OBJECT_ID_TYPE playerId) : UdpClientSendableEvent(playerId){
    }

    bool hasNextUserInput(){
        return (currentReadInputTick < (TICK_TYPE)playerInputs.size());
    }

    indexedPlayerInput getNextUserInput(){
        indexedPlayerInput retValue;
        retValue = playerInputs[currentReadInputTick];
        currentReadInputTick++;
        return retValue;
    }

    void addUserInput(indexedPlayerInput playerInput){
        playerInputs.push_back(playerInput);
    }
    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_USER_INPUT;
        packet << playerId;
        packet << (uint32_t) playerInputs.size();
        for(auto& input: playerInputs){
            packet << input;
        }
        return packet;
    }
};
