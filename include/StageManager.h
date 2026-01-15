#pragma once
#include "Stage.h"
#include <string>

class StageManager
{
public:
    static Stage loadStage(int16_t stageId);
    static std::vector<std::pair<int16_t, std::string>> loadStageList();

private:
    static std::string resolveStagePath(int16_t stageId);
};
