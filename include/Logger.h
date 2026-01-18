#include "plog/Logger.h"
#include "plog/Initializers/ConsoleInitializer.h"
#include "plog/Formatters/TxtFormatter.h"
#pragma once

#ifndef LOG_LEVEL
    #define LOG_LEVEL plog::warning
#endif

static plog::Logger<0>& initLogger(){
    static auto& logger = plog::init<plog::TxtFormatter, 0>(LOG_LEVEL, plog::streamStdOut);
    return logger;
}

static plog::Logger<1>& initAlwaysOnLogger(){
    static auto& logger = plog::init<plog::TxtFormatter, 1>(plog::verbose, plog::streamStdOut);
    return logger;
}