#pragma once

#include <SFML/System/Vector2.hpp>
#include "Types.h"
#include "GameState.h"
void updatePlayerVelocity(GameState& gameState, OBJECT_ID_TYPE id, sf::Vector2f velocity);
void updatePlayerLocation(GameState& gameState, OBJECT_ID_TYPE id, sf::Vector2f location);