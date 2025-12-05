#pragma once
#include "GameState.h"
#include "Operations/GameStateUpdater.h"

struct UpdateInfo{
public:
    //apply the Update to the given gameState using the GameStateUpdater
    //returns false if update is not applicable
    virtual bool applyUpdate(GameStateUpdater& gsUpdater, GameState & gameState) = 0;
    
};
