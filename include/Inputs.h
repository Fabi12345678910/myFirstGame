#pragma once
#include "Types.h"
#include <SFML/Network.hpp>

//OBJECT_ID_TYPE playerInput;

struct playerInput{
    bool moveLeft = false;
    bool moveRight = false;
    bool jump = false;
    bool projectile = false;
};

struct playerInputWithId{
    OBJECT_ID_TYPE playerId;
    struct playerInput playerInput;
};

struct allPlayerInputs{
    int ammountInputs;
    struct playerInputWithId *playerInputs;
};

inline sf::Packet& operator<<(sf::Packet& packet, const playerInput& input) {
    return packet << input.moveLeft
                  << input.moveRight
                  << input.jump
                  << input.projectile;
}

inline sf::Packet& operator>>(sf::Packet& packet, playerInput& input) {
    return packet >> input.moveLeft
                  >> input.moveRight
                  >> input.jump
                  >> input.projectile;
}

inline sf::Packet& operator<<(sf::Packet& packet, const playerInputWithId& inputId) {
    return packet << inputId.playerId << inputId.playerInput;
}

inline sf::Packet& operator>>(sf::Packet& packet, playerInputWithId& inputId) {
    return packet >> inputId.playerId >> inputId.playerInput;
}
