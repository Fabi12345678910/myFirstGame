#pragma once
#include "GameObject.h"
#include "Player.h"
#include "Stage.h"
#include "GameState.h"
#include "Operations/GameStateUpdater.h"

bool handlePlayerCollision(GameStateUpdater& gsUpdater, Player& player, Collidable& object, sf::RectangleShape& collisionPosition, sf::Vector2f playerVelocity, GameState& gameState);
