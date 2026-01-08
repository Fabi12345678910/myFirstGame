#include "plog/Logger.h"
#include "plog/Initializers/ConsoleInitializer.h"
#include "plog/Formatters/TxtFormatter.h"

#ifndef LOG_LEVEL
    #define LOG_LEVEL plog::verbose
#endif

static void initLogger(){
    plog::init<plog::TxtFormatter, 0>(LOG_LEVEL, plog::streamStdOut);
}