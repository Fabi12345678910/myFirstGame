#pragma once
#include "Player.h"
#include "Stage.h"

class GameState {
private:
    std::vector<Player> players;
    Stage stage;
};