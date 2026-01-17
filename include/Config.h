#pragma once
//this file is used for code-time configuration, e.g. some buffer sizes or whatever

#include <cstdint>
#define VERSION_TYPE std::uint16_t

constexpr uint16_t DEFAUL_PORT = 4444;

//0x.MM.mm.hh
//MM = major changes
//mm = minor changes
//hh = hotfixes
//hotfixes do not affect the api
//when major and minor versions are equal, a server should be able to work together with a client
constexpr uint16_t mapSelectionTime = 2000; //1k ms
constexpr VERSION_TYPE CONF_VERSION = 0x000901;
constexpr VERSION_TYPE CONF_API_VERSION = CONF_VERSION & 0xffff00;

//Debugging configuration, configurable via macros
#ifndef SHOW_CLIENT_HEALTH
    constexpr bool CONF_SHOW_CLIENT_HEALTH = true;
#else
    constexpr bool CONF_SHOW_CLIENT_HEALTH = SHOW_CLIENT_HEALTH;
#endif

#ifndef SEND_SERVER_HEALTH
    constexpr bool CONF_SEND_SERVER_HEALTH = true;
#else
    constexpr bool CONF_SEND_SERVER_HEALTH = SEND_SERVER_HEALTH;
#endif

#ifndef SHOW_SERVER_HEALTH
    constexpr bool CONF_SHOW_SERVER_HEALTH = true;
#else
    constexpr bool CONF_SHOW_SERVER_HEALTH = SHOW_SERVER_HEALTH;
#endif

//Debug config:
constexpr bool debugClient = true;
constexpr bool debugClientGameStore = debugClient && false;
constexpr bool debugClientFrameGen = debugClient && false;
constexpr bool debugClientNetworking = debugClient && false;
constexpr bool debugClientInputs = debugClient && false;
constexpr bool debugClientState = debugClient && false;
constexpr bool debugClientPerformance = debugClient && false;

constexpr bool debugServer = true;
constexpr bool debugServerInputProcessing = debugServer && false;
constexpr bool debugServerInputQueues = debugServer && false;
constexpr bool debugServerNetworking = debugServer && true;
constexpr bool debugServerGenerations = debugServer && false;
constexpr bool debugServerGameState = debugServer && true;

constexpr bool debugCollision = false;


//Timings
constexpr uint16_t mapSelectionTimeS = 20;
constexpr uint16_t mapStartTimeS = 5;
constexpr uint16_t selectionTimeoutMs = 100;