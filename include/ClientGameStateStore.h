#pragma once
#include "UpdateInfo.h"
#include "GameState.h"
#include "CircularArray.h"
#include "Inputs.h"
#include "Operations/GameStateUpdater.h"
#include "Operations/LocalPlayerGameStateUpdater.h"
#include "Operations/ClientGameStateUpdater.h"
#include "GameUpdate.h"


#include <iostream>

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
            std::cout << "pushing old gamestate\n";
            gameStates.push(gameStates.back());
            gameStates.back().gameStateUpdated = false;
            gameStates.back().updateInfos.clear();
            gameStates.back().updateInfosFinalized = false;
            gameStates.back().localInputFinalized = false;
            gameStates.back().hasSnapshot = false;
        }
        return true;
    }

    bool updateGameState(TICK_TYPE tick){
        std::cout << tick << ": updateGameState()\n";
        if(!isGameStateAvailable(tick)){return false;}
        if(gameStates[tick].gameStateUpdated){return true;}
        //well here we're actually updating the gameState
        ClientGameStateUpdater gsUpdater(gameStates[tick].gameState, localPlayerId, gameStates[tick].localPlayer);

        if(gameStates[tick].hasSnapshot){
            std::cout << "applying snapshot update\n";
            for (UpdateInfo* updateInfo: gameStates[tick].updateInfos){
                if(updateGameState(tick-1)){
                    gameStates[tick].gameState = gameStates[tick-1].gameState;
                }
                updateInfo->applyUpdate(gsUpdater, gameStates[tick].gameState);
                //still update LocalPlayer tho, since he is not included in the snapshot
            }
            if(gsUpdater.isLocalPlayerAdded()){
                gameStates[tick].localPlayerInitialized = true;
            }
            updateLocalPlayer(tick);
            gameStates[tick].gameStateUpdated = true;
            return true;
        }

        if(!gameStates[tick].updateInfosFinalized){return false;}
        if(!gameStates[tick].localInputFinalized){return false;}
        if(!updateGameState(tick-1)){return false;}

        gameStates[tick].gameState = gameStates[tick-1].gameState;
        gameStates[tick].localPlayer = gameStates[tick-1].localPlayer;
        gameStates[tick].localPlayerInitialized = gameStates[tick-1].localPlayerInitialized;

        std::cout << "applying client side update\n";
        for (UpdateInfo* updateInfo: gameStates[tick].updateInfos)
        {
            updateInfo->applyUpdate(gsUpdater, gameStates[tick].gameState);
        }
        if(gsUpdater.isLocalPlayerAdded()){
            gameStates[tick].localPlayerInitialized = true;
        }

        std::cout << "updating gameState " << tick << " with tickrate of " << this->tickrateSeconds << '\n';
        updateGame(gsUpdater, gameStates[tick].gameState, this->tickrateSeconds);
        updateLocalPlayer(tick);
        std::cout << "updated gameState " << tick << '\n';
        gameStates[tick].gameStateUpdated = true;
        return true;
    }

    bool updateLocalPlayer(TICK_TYPE tick){
        std::cout << tick << ": updateLocalPlayer()\n";
        if(!isGameStateAvailable(tick)){return false;}
        if(!isGameStateAvailable(tick-1)){return false;}
        if(!gameStates[tick].localInputFinalized){return false;};
        if(!gameStates[tick].localPlayerInitialized && gameStates[tick-1].localPlayerInitialized){
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
        std::cout << "adding gsUpdateInfo\n";
        if(!ensureGameStateIsAvailable(tick)){
            return false;
        }
        if(!gameStates[tick].updateInfosFinalized){
            std::cout << "setting hasSnapshot to true\n";
            gameStates[tick].hasSnapshot = true;
            gameStates[tick].updateInfos.push_back(new gsUpdateInfo(updateInfo));
        }else{
            std::cout << "update already finalized\n";
        }
        return true;
    }

    bool addUpdateInfo(TICK_TYPE tick, playerInputWithId& updateInfo){
        std::cout << "adding playerInput\n";
        if(!ensureGameStateIsAvailable(tick)){
            return false;
        }
        if(!gameStates[tick].updateInfosFinalized){
            gameStates[tick].updateInfos.push_back(new playerInputWithId(updateInfo));
        }else{
            std::cout << "update already finalized\n";
        }
        return true;
    }

    bool finalizeUpdateInfos(TICK_TYPE tick){
        std::cout << "finalizing updateInfos\n";
        if (isGameStateAvailable(tick))
        {
            gameStates[tick].updateInfosFinalized = true;
        }
        return true;
    }

    bool setLocalInput(TICK_TYPE tick, playerInput input){
        std::cout << "finalizing updateInfos\n";
        if (!isGameStateAvailable(tick))
        {
            return false;
        }
        gameStates[tick].localInput = input;
        return true;
    }
    bool finalizeLocalInput(TICK_TYPE tick){
        std::cout << "finalizing updateInfos\n";
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
            std::cout << tick << " tick not available\n"; 
            return NULL;
        }
        if(!gameStates[tick].localPlayerInitialized){
            std::cout << "localPlayer is uninitialized, not including in result\n";
        }
        if(gameStates[tick].gameStateUpdated){
            std::cout << tick << " gamestate already rendered\n";
            if(localPlayer != NULL && gameStates[tick].localPlayerInitialized){*localPlayer = &gameStates[tick].localPlayer;}
            return &(gameStates[tick].gameState);
        }else{
            if(updateIfPossible){
                std::cout << tick << " trying to update gs\n";
                if(updateGameState(tick)){
                    if(localPlayer != NULL && gameStates[tick].localPlayerInitialized){*localPlayer = &gameStates[tick].localPlayer;}
                    return &(gameStates[tick].gameState);
                }else{
                    return NULL;
                }
            }else{
                std::cout << tick << " not updating gs\n";
                return NULL;
            }
        }
    }
};
