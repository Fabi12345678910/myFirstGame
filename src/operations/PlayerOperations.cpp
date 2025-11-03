#include "PlayerOperations.h"

void updatePlayerVelocity(GameState& gameState, OBJECT_ID_TYPE id, sf::Vector2f velocity){
    gameState.getPlayer(id).setVelocity(velocity);
}

void updatePlayerLocation(GameState& gameState, OBJECT_ID_TYPE id, sf::Vector2f location){
    gameState.getPlayer(id).setPosition(location);
}