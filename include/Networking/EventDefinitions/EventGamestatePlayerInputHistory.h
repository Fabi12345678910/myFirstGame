#pragma once

#include "Networking/Event.h"
#include "Networking/EventTypeList.h"
#include "Inputs.h"
#include "GameState.h"
#include <iterator>
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

struct UpdateInfo{
    enum activeComponent{GAMESTATE_PLAYER_INPUT, PLAYER_INPUT};
    gsUpdateInfo gsUpdate;
    std::vector<playerInputWithId> pInput;

    UpdateInfo(){}
    UpdateInfo(gsUpdateInfo gsUpdate) : gsUpdate(gsUpdate){}
    UpdateInfo(std::vector<playerInputWithId> pInput) : pInput(pInput){}
};

struct LabeledUpdateInfo{
    UpdateInfo::activeComponent type;
    UpdateInfo& info;
    LabeledUpdateInfo(UpdateInfo::activeComponent type,
    UpdateInfo& info): type(type), info(info){}
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
    PLAYER_INPUT_NO_TYPE latestAcknowledgedPlayerInput;
    std::vector<UpdateInfo> updateInfos;

    bool hasNextInfo(){
        return nextInfoIndex < updateInfos.size();
    }

    LabeledUpdateInfo getNextInfo(){
        nextInfoIndex++;
        if((nextInfoIndex-1) % snapShotDistance == 0){
            return LabeledUpdateInfo(UpdateInfo::GAMESTATE_PLAYER_INPUT, updateInfos[nextInfoIndex - 1]);
        }else{
            return LabeledUpdateInfo(UpdateInfo::PLAYER_INPUT, updateInfos[nextInfoIndex - 1]);
        }
    }

    EventGamestatePlayerInputHistory(sf::Packet packet){
        uint32_t size;
        if(!(packet >> startingGameTick >> snapShotDistance >> latestAcknowledgedPlayerInput >> size)){
            throw std::runtime_error("failed to read updateInfos metadata");
        }
        updateInfos.reserve(size);
        for (size_t i = 0; i < size; i++)
        {
            updateInfos.emplace_back();
            if(i%snapShotDistance == 0){
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

    std::vector<playerInputWithId>& createCombinedUpdateInfo(GameState& gameState){
        auto& updateInfo = updateInfos.emplace_back();
        for (Player& p : gameState.getPlayers()){
            updateInfo.gsUpdate.playerInfos.emplace_back(p);
            std::cout << "added player " << p.getId() <<" to playerInfos\n";
        }
        for (Projectile &p : gameState.getProjectiles()){
            updateInfo.gsUpdate.projectileInfos.emplace_back(p);
        }
        return updateInfo.pInput;
    }

    std::vector<playerInputWithId>& createNewPlayerInputs(){
        auto& updateInfo = updateInfos.emplace_back((std::vector<playerInputWithId>){});
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
        packet << latestAcknowledgedPlayerInput;
        packet << static_cast<uint32_t>(updateInfos.size());

        for (size_t i = 0; i < updateInfos.size(); i++)
        {
            if(i%snapShotDistance == 0){
                packet << updateInfos[i].gsUpdate;
                packet << updateInfos[i].pInput;
            }else{
                packet << updateInfos[i].pInput;
            }
        }
        
        return packet;
    }
};
