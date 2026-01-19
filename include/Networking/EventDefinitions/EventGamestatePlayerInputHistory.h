#pragma once

#include "Networking/Event.h"
#include "Networking/EventTypeList.h"
#include "Inputs.h"
#include "GameState.h"
#include "GameStateUpdates.h"

inline sf::Packet& operator <<(
    sf::Packet& packet,
    const std::vector<playerInputWithId>& vec)
{
    packet << static_cast<uint32_t>(vec.size());

    for (const auto& pInput : vec)
    {
        packet << pInput;
    }

    return packet;
}

inline sf::Packet& operator >>(
    sf::Packet& packet,
    std::vector<playerInputWithId>& vec)
{
    uint32_t count;
    packet >> count;

    vec.resize(count);

    for (uint32_t i = 0; i < count; ++i)
    {
        packet >> vec[i];;
    }

    return packet;
}

struct UpdateInfo1{
    enum activeComponent{GAMESTATE_PLAYER_INPUT, PLAYER_INPUT};
    gsUpdateInfo gsUpdate;
    std::vector<playerInputWithId> pInput;

    UpdateInfo1(){}
    UpdateInfo1(gsUpdateInfo gsUpdate) : gsUpdate(gsUpdate){}
    UpdateInfo1(std::vector<playerInputWithId> pInput) : pInput(pInput){}
};

struct LabeledUpdateInfo{
    UpdateInfo1::activeComponent type;
    UpdateInfo1& info;
    LabeledUpdateInfo(UpdateInfo1::activeComponent type,
    UpdateInfo1& info): type(type), info(info){}
};

//this will send the latest snapshots and the inputs used to create those snapshots
class EventGamestatePlayerInputHistory:public Event
{
private:
    size_t nextInfoIndex = 0;
    const GameState* baseLineGameState = NULL;
public:
    // The Tick on which the first playerInput is based
    TICK_TYPE startingGameTick;
    std::uint8_t snapShotDistance;
    std::vector<UpdateInfo1> updateInfos;

    bool hasNextInfo(){
        return nextInfoIndex < updateInfos.size();
    }

    LabeledUpdateInfo getNextInfo(){
        nextInfoIndex++;
        if(snapShotDistance == 255){
            return LabeledUpdateInfo(UpdateInfo1::PLAYER_INPUT, updateInfos[nextInfoIndex - 1]);
        }else if((nextInfoIndex-1) % snapShotDistance == 0){
            return LabeledUpdateInfo(UpdateInfo1::GAMESTATE_PLAYER_INPUT, updateInfos[nextInfoIndex - 1]);
        }else{
            return LabeledUpdateInfo(UpdateInfo1::PLAYER_INPUT, updateInfos[nextInfoIndex - 1]);
        }
    }

    EventGamestatePlayerInputHistory(sf::Packet packet){
        uint32_t size;
        if(!(packet >> startingGameTick >> snapShotDistance >> size)){
            throw std::runtime_error("failed to read updateInfos metadata");
        }
        updateInfos.reserve(size);
        for (size_t i = 0; i < size; i++)
        {
            updateInfos.emplace_back();
            if(snapShotDistance == 255){
                packet >> updateInfos[i].pInput;
            }
            else if(i%snapShotDistance == 0){
                packet >> updateInfos[i].gsUpdate;
                packet >> updateInfos[i].pInput;
            }else{
                packet >> updateInfos[i].pInput;
            }
        }

    };
    EventGamestatePlayerInputHistory() {}

    //sets a gameState which is to be expected by the client, will not be sent
    //if set, even the first sent gameState will only require delta Updates
    void setBaseLineGameState(const GameState* gameState){
        baseLineGameState = gameState;
    }

    UpdateInfo1& createCombinedUpdateInfo(GameState& gameState, TICK_TYPE latestIncludedPlayerInput){
        UpdateInfo1& updateInfo = updateInfos.emplace_back();
        for (auto it = gameState.getPlayersBegin(); it != gameState.getPlayersEnd(); it++){
            updateInfo.gsUpdate.playerInfos.emplace_back(it->second);
        }
        for(auto projectileIt = gameState.getProjectilesBegin(); projectileIt != gameState.getProjectilesEnd(); projectileIt++){
            updateInfo.gsUpdate.projectileInfos.emplace_back(projectileIt->second);
        }
        updateInfo.gsUpdate.latestIncludedPInput = latestIncludedPlayerInput;
        return updateInfo;
    }

    std::vector<playerInputWithId>& createNewPlayerInputs(){
        UpdateInfo1& updateInfo = updateInfos.emplace_back();
        updateInfo.pInput = std::vector<playerInputWithId>();
        return updateInfo.pInput;
    }

    void addPlayerInputs(std::vector<playerInputWithId> pInput){
        auto& updateInfo = updateInfos.emplace_back(pInput);
    }

    sf::Packet toPacket() const override{
        sf::Packet packet;
        packet << (DATATYPE_EVENT_TYPE) EVENT_TYPE_GAMESTATE_PLAYERINPUT_HISTORY;
        packet << startingGameTick;
        packet << snapShotDistance;
        packet << static_cast<uint32_t>(updateInfos.size());

        for (size_t i = 0; i < updateInfos.size(); i++)
        {
            if(snapShotDistance == 255){
                packet << updateInfos[i].pInput;
            }
            else if(i%snapShotDistance == 0){
                packet << updateInfos[i].gsUpdate;
                packet << updateInfos[i].pInput;
            }else{
                packet << updateInfos[i].pInput;
            }
        }
        
        return packet;
    }
};
