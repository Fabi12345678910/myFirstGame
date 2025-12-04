#pragma once
#include "GameState.h"
#include "CircularArray.h"
#include "Inputs.h"

struct StoreState{
    enum State{
        UNINITIALIZED, READY_FOR_GENERATION, GENERATED
    };
    State state;
    Player localPlayer;
    playerInput localInput;
    GameState gameState;
    std::vector<struct playerInputWithId> playerInputs;
    StoreState() : state(UNINITIALIZED){}
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
    //returns false if GameState could not be made available(e.g. is too old)
    bool ensureGameStateIsAvailable(TICK_TYPE tick){
        if(tick < gameStates.getMinIndex()) {return false;}
        if(tick < gameStates.getSize()) {return true;}

        while (gameStates.getSize() <= tick){
            gameStates.push(gameStates[tick]);
        }
        return true;
    }

public:
    ClientGameStateStore(size_t initialGameStates, GameState& initialGS){
        gameStates.push(StoreState());
        gameStates[0].gameState = initialGS;
        for (size_t i = 1; i < initialGameStates; i++)
        {
            gameStates.push(gameStates.back());
        }
    }

    GameState& getGameState(TICK_TYPE tick){
        //generate 
    }

    ClientGameStateStore(size_t initialGameStates){
        gameStates.push(StoreState());
        for (size_t i = 1; i < initialGameStates; i++)
        {
            gameStates.push(gameStates.back());
        }
    }

    ~ClientGameStateStore(){}

    bool insertPlayerInput(TICK_TYPE tick, playerInputWithId& input){
        bool gsAvailable = ensureGameStateIsAvailable(tick);
        if(!gsAvailable){
            return false;
        }
        gameStates[tick].playerInputs.emplace_back(input);
        return true;
    }

    bool setPlayerInputs(TICK_TYPE tick, std::vector<playerInputWithId>& inputs){
        bool gsAvailable = ensureGameStateIsAvailable(tick);
        if(!gsAvailable){
            return false;
        }
        gameStates[tick].playerInputs = inputs;
        return true;
    }

    bool applyGameStateUpdate(TICK_TYPE tick, gsUpdateInfo& gameState){
        bool gsAvailable = ensureGameStateIsAvailable(tick);
        if(!gsAvailable){
            return false;
        }
        gameState.applyUpdate(gameStates[tick].gameState);
        return true;
    }
    bool isGameStateAvailable(TICK_TYPE tick){
        return (tick >= gameStates.getMinIndex() && tick < gameStates.getSize());
    }
    std::optional<GameState&> getGameState(TICK_TYPE tick, bool generateIfNecessary){
        if(!isGameStateAvailable(tick)){
            return std::nullopt;
        }
        return gameStates[tick].gameState;
    }

    bool addPlayer(TICK_TYPE tick, Player &&player, bool applyToFurtherGameStates = true){
        if(!ensureGameStateIsAvailable(tick)){
            return false;
        }
        gameStates[tick].gameState.addPlayer(player);
        if(applyToFurtherGameStates){
            for (TICK_TYPE i = tick + 1; i < gameStates.getSize(); i++)
            {
                gameStates[i].gameState.addPlayer(player);
            }
            
        }
    }
};
