#pragma once
//this file is used for code-time configuration, e.g. some buffer sizes or whatever

#include <cstdint>

#define VERSION_TYPE std::uint16_t

//0x.MM.mm.hh
//MM = major changes
//mm = minor changes
//hh = hotfixes
//hotfixes do not affect the api
//when major and minor versions are equal, a server should be able to work together with a client
constexpr VERSION_TYPE VERSION = 0x000600;
constexpr VERSION_TYPE API_VERSION = VERSION & 0xffff00;

constexpr uint16_t mapSelectionTime = 2000; //1k ms
