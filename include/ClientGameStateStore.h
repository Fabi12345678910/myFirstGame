#pragma once
#include "UpdateInfo.h"
#include "GameState.h"
#include "CircularArray.h"
#include "Inputs.h"
#include "Operations/LocalPlayerGameStateUpdater.h"
#include "Operations/ClientGameStateUpdater.h"
#include "GameUpdate.h"
#include "plog/Log.h"
#include "GameStateUpdates.h"
#include "Config.h"

#include "GameStateHealth.h"

struct StoreState{
    std::vector<UpdateInfo*> updateInfos = std::vector<UpdateInfo*>();
    bool updateInfosFinalized = false;
    bool hasSnapshot = false;
    Player localPlayer = Player();
    bool localPlayerInitialized = false;
    playerInput localInput = playerInput();
    bool localInputFinalized = false;
    GameState gameState = GameState();
    bool gameStateUpdated = false;
    StoreState() {}
};


#if true //can be changed to false to get vscode linting
    template<std::size_t N>
#else
    static constexpr int N = 32;
#endif
class ClientGameStateStore
{
private:
    CircularArray<StoreState, N> gameStates;
    float tickrateSeconds = 0;
    OBJECT_ID_TYPE localPlayerId = 0;
    //returns false if GameState could not be made available(e.g. is too old)
    bool ensureGameStateIsAvailable(TICK_TYPE tick){
        if(tick < gameStates.getMinIndex()) {return false;}
        if(tick < gameStates.getSize()) {return true;}

        while (gameStates.getSize() <= tick){
            gameStates.push(gameStates.back());
            gameStates.back().gameStateUpdated = false;
            gameStates.back().updateInfos.clear();
            gameStates.back().updateInfosFinalized = false;
            gameStates.back().localInputFinalized = false;
            gameStates.back().hasSnapshot = false;
        }
        return true;
    }

    bool checkForDiff(float const & local, float const& correct, float acceptableDiff){
        float diff = correct-local;
        if(diff < 0){diff = -diff;} //diff = abs(diff)
        return diff > acceptableDiff;        
    }

    //returns true if localPlayer was changed
    bool correctLocalPlayer(TICK_TYPE tick, Player& correctPlayer){
        if(!isGameStateAvailable(tick)){
            //todo maybe handle things different if localPlayer is not available
            return false;
        }
        Player& localPlayer = gameStates[tick].localPlayer;
        bool somethingChanged = false;
        somethingChanged |= checkForDiff(localPlayer.getPosition().x, correctPlayer.getPosition().x, 1);
        somethingChanged |= checkForDiff(localPlayer.getPosition().y, correctPlayer.getPosition().y, 1);
        somethingChanged |= checkForDiff(localPlayer.getVelocity().x, correctPlayer.getVelocity().x, 1);
        somethingChanged |= checkForDiff(localPlayer.getVelocity().y, correctPlayer.getVelocity().y, 1);
        if(somethingChanged){
            localPlayer = correctPlayer;
        }
        return somethingChanged;
    }

    bool applySnapshot(TICK_TYPE tick){
        ClientGameStateUpdater gsUpdater(gameStates[tick].gameState, localPlayerId, gameStates[tick].localPlayer);
        PLOG_DEBUG_IF(debugClientGameStore) << "applying snapshot update";
        bool snapShotFound = false;
        for (UpdateInfo* updateInfo: gameStates[tick].updateInfos){
            gsUpdateInfo* snapshot = dynamic_cast<gsUpdateInfo*>(updateInfo);
            if(snapshot == NULL){continue;}
            PLOG_DEBUG_IF(debugClientGameStore) << "found the snapshot update";
            if(updateGameState(tick-1)){
                gameStates[tick].gameState = gameStates[tick-1].gameState;
            }
            PLOG_DEBUG_IF(debugClientGameStore) << "applying the snapshot update";
            snapshot->applyUpdate(gsUpdater, gameStates[tick].gameState);
            PLOG_DEBUG_IF(debugClientGameStore) << "applied the snapshot update";
            if(snapshot->latestIncludedPInput != 0-1){
                try{
                    Player& localPlayer = gameStates[tick].gameState.getPlayer(localPlayerId);
                    if(correctLocalPlayer(snapshot->latestIncludedPInput, gameStates[tick].gameState.getPlayer(localPlayerId))){
                        PLOG_INFO_IF(debugClientGameStore) << "localPlayer differed significantly!!";
                        for (TICK_TYPE i = snapshot->latestIncludedPInput + 1; i < gameStates.getSize(); i++)
                        {
                            updateLocalPlayer(i, true);
                        }
                    }
                }
                catch(const std::exception& e)
                {
                    PLOG_WARNING << e.what();
                }
            }else{
                PLOG_INFO_IF(debugClientGameStore) << "got no latestIncludedPInput"; 
            }
            snapShotFound = true;
        }
        if(!snapShotFound){return false;}
        //still update LocalPlayer tho, since he is not included in the snapshot
        if(gsUpdater.isLocalPlayerAdded()){
            gameStates[tick].localPlayerInitialized = true;
        }
        updateLocalPlayer(tick);
        gameStates[tick].gameStateUpdated = true;
        return true;
    }

    bool updateGameState(TICK_TYPE tick){
        if(!isGameStateAvailable(tick)){return false;}
        if(gameStates[tick].gameStateUpdated){return true;}
        //well here we're actually updating the gameState
        ClientGameStateUpdater gsUpdater(gameStates[tick].gameState, localPlayerId, gameStates[tick].localPlayer);

        if(gameStates[tick].hasSnapshot){
            applySnapshot(tick);
        }

        if(!gameStates[tick].updateInfosFinalized){return false;}
        if(!gameStates[tick].localInputFinalized){return false;}
        if(!updateGameState(tick-1)){return false;}

        gameStates[tick].gameState = gameStates[tick-1].gameState;
        gameStates[tick].localPlayer = gameStates[tick-1].localPlayer;
        gameStates[tick].localPlayerInitialized = gameStates[tick-1].localPlayerInitialized;

        for (UpdateInfo* updateInfo: gameStates[tick].updateInfos)
        {
            updateInfo->applyUpdate(gsUpdater, gameStates[tick].gameState);
        }
        if(gsUpdater.isLocalPlayerAdded()){
            gameStates[tick].localPlayerInitialized = true;
        }

        updateGame(gsUpdater, gameStates[tick].gameState, this->tickrateSeconds);
        updateLocalPlayer(tick);
        gameStates[tick].gameStateUpdated = true;
        return true;
    }

    bool updateLocalPlayer(TICK_TYPE tick, bool forceUpdate = false){
        if(!isGameStateAvailable(tick)){return false;}
        if(!isGameStateAvailable(tick-1)){return false;}
        if(!gameStates[tick].localInputFinalized){return false;};
        if((forceUpdate || !gameStates[tick].localPlayerInitialized) && gameStates[tick-1].localPlayerInitialized){
            gameStates[tick].localPlayer = gameStates[tick-1].localPlayer;
            gameStates[tick].localPlayerInitialized = true;
        }

        LocalPlayerGameStateUpdater gsUpdaterLocalPlayer(gameStates[tick].gameState);
        gameStates[tick].localInput.applyUpdate(gameStates[tick].localPlayer, gsUpdaterLocalPlayer, gameStates[tick].gameState);
        updateGameSinglePlayer(gsUpdaterLocalPlayer, gameStates[tick].gameState, gameStates[tick].localPlayer, this->tickrateSeconds);
        return true;
    }

public:
    bool hasLocalInput(TICK_TYPE tick){
        if(!isGameStateAvailable(tick)){return false;}
        if(gameStates[tick].localInputFinalized){
            return true;
        }else{
            return false;
        }
    }
    playerInput getLocalInput(TICK_TYPE tick){
        if(hasLocalInput(tick)){
            return gameStates[tick].localInput;
        }else{
            throw std::runtime_error("localInput unavailable");
        }
    }

    void setLocalPlayerId(OBJECT_ID_TYPE id){
        this->localPlayerId = id;
    }

    bool addPlayer(TICK_TYPE tick, Player const& player){
        if(!ensureGameStateIsAvailable(tick)){
            return false;
        }
        gameStates[tick].localPlayer = player;
    }

    void setTickrate(float tickrateSeconds){
        this->tickrateSeconds = tickrateSeconds;
    }

    ClientGameStateStore(size_t initialGameStates, GameState& initialGS){
        gameStates.push(StoreState());
        gameStates[0].gameState = initialGS;
        for (size_t i = 1; i < initialGameStates; i++)
        {
            gameStates.push(gameStates.back());
        }
    }

    ClientGameStateStore(size_t initialGameStates){
        gameStates.push(StoreState());
        for (size_t i = 1; i < initialGameStates; i++)
        {
            gameStates.push(gameStates.back());
        }
    }

    ~ClientGameStateStore(){}

    bool addUpdateInfo(TICK_TYPE tick, gsUpdateInfo& updateInfo){
        if(!ensureGameStateIsAvailable(tick)){
            return false;
        }
        if(!gameStates[tick].updateInfosFinalized){
            gameStates[tick].hasSnapshot = true;
            gameStates[tick].updateInfos.push_back(new gsUpdateInfo(updateInfo));
        }else{
            PLOG_INFO_IF(debugClientGameStore) << "update already finalized";
        }
        return true;
    }

    bool addUpdateInfo(TICK_TYPE tick, playerInputWithId& updateInfo){
        if(!ensureGameStateIsAvailable(tick)){
            return false;
        }
        if(!gameStates[tick].updateInfosFinalized){
            gameStates[tick].updateInfos.push_back(new playerInputWithId(updateInfo));
        }else{
            PLOG_INFO_IF(debugClientGameStore) <<  "update already finalized";
        }
        return true;
    }

    bool finalizeUpdateInfos(TICK_TYPE tick){
        if (isGameStateAvailable(tick))
        {
            gameStates[tick].updateInfosFinalized = true;
        }
        return true;
    }

    bool setLocalInput(TICK_TYPE tick, playerInput input){
        if (!isGameStateAvailable(tick))
        {
            return false;
        }
        gameStates[tick].localInput = input;
        return true;
    }
    bool finalizeLocalInput(TICK_TYPE tick){
        if (!isGameStateAvailable(tick))
        {
            return false;
        }
        gameStates[tick].localInputFinalized = true;
        return true;
    }

    bool areUpdateInfosFinalized(TICK_TYPE tick){
        if (isGameStateAvailable(tick))
        {
            return gameStates[tick].updateInfosFinalized;
        }
        return false;
    }

    bool isGameStateAvailable(TICK_TYPE tick){
        return (tick >= gameStates.getMinIndex() && tick < gameStates.getSize());
    }

    //returns a ptr to the GameState or NULL
    GameState* getGameState(TICK_TYPE tick, bool updateIfPossible, Player** localPlayer){
        if(!isGameStateAvailable(tick)){
            return NULL;
        }
        if(!gameStates[tick].localPlayerInitialized){
        }
        if(gameStates[tick].gameStateUpdated){
            PLOG_DEBUG_IF(debugClientGameStore) << tick << " gamestate already rendered";
            if(localPlayer != NULL && gameStates[tick].localPlayerInitialized){*localPlayer = &gameStates[tick].localPlayer;}
            return &(gameStates[tick].gameState);
        }else{
            if(updateIfPossible){
                if(updateGameState(tick)){
                    if(localPlayer != NULL && gameStates[tick].localPlayerInitialized){*localPlayer = &gameStates[tick].localPlayer;}
                    return &(gameStates[tick].gameState);
                }else{
                    return NULL;
                }
            }else{
                return NULL;
            }
        }
    }

    HealthReport getHealthReport(TICK_TYPE currentTickToDisplay){
        HealthReport report;
        report.tickHealths.reserve(30);
        for (TICK_TYPE i = currentTickToDisplay - 9; i <= currentTickToDisplay + 20; i++)
        {
            if (!isGameStateAvailable(i)){
                report.tickHealths.emplace_back(i, TickHealth::UNAVAILABLE);
                continue;
            }
            if(gameStates[i].gameStateUpdated){
                report.tickHealths.emplace_back(i, TickHealth::GENERATED);
                continue;
            }
            if(gameStates[i].updateInfosFinalized){
                report.tickHealths.emplace_back(i, TickHealth::READY_TO_GENERATE);
                continue;
            }else{
                report.tickHealths.emplace_back(i, TickHealth::MISSING_SERVER_UPDATE_INFOS);
            }            
        }
        return report;
    }
};
