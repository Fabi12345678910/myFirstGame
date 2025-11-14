#pragma once
#include "GameObject.h"
#include "Player.h"
#include "Stage.h"

void handlePlayerCollision(Player& player, Collidable& object, float prevPlayerBottomY, Stage const& gameState);
