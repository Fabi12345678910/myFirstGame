#pragma once
#include "UpdateInfo.h"
#include "GameState.h"
#include "CircularArray.h"
#include "Inputs.h"
#include "Operations/GameStateUpdater.h"
#include "Operations/ClientGameStateUpdater.h"
#include "GameUpdate.h"


#include <iostream>

struct StoreState{
    std::vector<UpdateInfo*> updateInfos = std::vector<UpdateInfo*>();
    bool updateInfosFinalized = false;
    Player localPlayer = Player();
    bool hasSnapshot = false;
    playerInput localInput = playerInput();
    GameState gameState = GameState();
    bool gameStateUpdated = false;
    StoreState() {}
};


#ifndef VSCODE_LINT
    template<std::size_t N>
#else
    static constexpr int N = 32;
#endif
class ClientGameStateStore
{
private:
    CircularArray<StoreState, N> gameStates;
    float tickrateSeconds = 0.00001;
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
            gameStates.back().hasSnapshot = false;
        }
        return true;
    }

    bool updateGameState(TICK_TYPE tick, GameStateUpdater& gsUpdater){
        std::cout << tick << ": updateGameState()\n";
        if(!isGameStateAvailable(tick)){return false;}
        if(gameStates[tick].gameStateUpdated){return true;}
        //well here we're actually updating the gameState

        if(gameStates[tick].hasSnapshot){
            std::cout << "applying snapshot update\n";
            for (UpdateInfo* updateInfo: gameStates[tick].updateInfos){
                updateInfo->applyUpdate(gsUpdater, gameStates[tick].gameState);
            }
            gameStates[tick].gameStateUpdated = true;
            return true;
        }

        if(!updateGameState(tick-1, gsUpdater)){return false;}
        if(!gameStates[tick].updateInfosFinalized){return false;}
        
        gameStates[tick].gameState = gameStates[tick-1].gameState;
        std::cout << "applying client side update\n";
        for (UpdateInfo* updateInfo: gameStates[tick].updateInfos)
        {
            updateInfo->applyUpdate(gsUpdater, gameStates[tick].gameState);
        }
        ClientGameStateUpdater gsUpdaterTimeStep(gameStates[tick].gameState);

        std::cout << "updating gameState " << tick << " with tickrate of " << this->tickrateSeconds << '\n';
        updateGame(gsUpdaterTimeStep, gameStates[tick].gameState, this->tickrateSeconds);
        
        std::cout << "updated gameState " << tick << '\n';
        gameStates[tick].gameStateUpdated = true;
        return true;
    }

public:
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
    GameState* getGameState(TICK_TYPE tick, bool updateIfPossible){
        if(!isGameStateAvailable(tick)){
            std::cout << tick << " tick not available\n"; 
            return NULL;
        }
        if(gameStates[tick].gameStateUpdated){
            std::cout << tick << " gamestate already rendered\n";
            return &(gameStates[tick].gameState);
        }else{
            if(updateIfPossible){
                std::cout << tick << " trying to update gs\n";
                ClientGameStateUpdater gsUpdater(gameStates[tick].gameState);
                if(updateGameState(tick, gsUpdater)){
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
