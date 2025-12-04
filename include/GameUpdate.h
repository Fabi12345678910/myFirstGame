#include "GameState.h"
#include "Operations/GameStateUpdater.h"
#include "Inputs.h"

//update the gameState, after deltaTime time has passed
void updateGame(GameStateUpdater& gsUpdater, std::vector<playerInputWithId> inputs, GameState& gameState, float deltaTime);