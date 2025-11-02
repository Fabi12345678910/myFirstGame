#include "PlayerOperations.h"

void updateVelocity(GameState& gameState, OBJECT_ID_TYPE id, sf::Vector2f velocity){
    gameState.getPlayer(id).setVelocity(velocity);
}