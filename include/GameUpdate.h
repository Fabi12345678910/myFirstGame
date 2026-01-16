#include "GameState.h"
#include "Operations/GameStateUpdater.h"

//update the gameState, after deltaTime time has passed
void updateGame(GameStateUpdater& gsUpdater, GameState& gameState, float deltaTime);
void updateGameSinglePlayer(GameStateUpdater& gsUpdater, GameState& gameState, Player& player, float deltaTime);