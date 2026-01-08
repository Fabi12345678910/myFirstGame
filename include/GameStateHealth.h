#pragma once
#include "Types.h"

#include <vector>

struct TickHealth{
    TICK_TYPE tick = 0;
    enum HealthState{
        GENERATED,
        READY_TO_GENERATE,
        MISSING_SERVER_UPDATE_INFOS,
        UNAVAILABLE
    } healthState = UNAVAILABLE;
    TickHealth(){};
    TickHealth(TICK_TYPE tick, HealthState healthState): tick(tick), healthState(healthState){};
};

struct HealthReport{
    std::vector<TickHealth> tickHealths;
};