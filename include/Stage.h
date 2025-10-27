#pragma once
#include "StageObject.h"

class Stage {
    std::vector<StageObject> stageObjects;
    std::vector<sf::Vector2f> spawnPoints;
};