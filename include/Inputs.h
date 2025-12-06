#pragma once
#include "Types.h"
#include <SFML/Network.hpp>
#include "UpdateInfo.h"
#include <iostream>

//OBJECT_ID_TYPE playerInput;

struct playerInput{
    bool moveLeft = false;
    bool moveRight = false;
    bool jump = false;
    bool projectile = false;
    bool applyUpdate(Player& player, GameStateUpdater& gsUpdater, GameState & gameState){
        OBJECT_ID_TYPE playerId = player.getId();
        sf::Vector2f playerVelocity = player.getVelocity();
        if(moveLeft){
            playerVelocity.x = -player.getSpeed();
            gsUpdater.setPlayerVelocity(player, playerVelocity);
            gsUpdater.setPlayerFacing(player, Player::FACING_LEFT);
        }
        if(moveRight){
            playerVelocity.x = player.getSpeed();
            gsUpdater.setPlayerVelocity(player, playerVelocity);
            gsUpdater.setPlayerFacing(player, Player::FACING_RIGHT);
        }
        if(!moveLeft&&!moveRight){
            playerVelocity.x = 0;
            gsUpdater.setPlayerVelocity(player, playerVelocity);
        }
        if(jump){
            if(player.getIsOnGround()){
                playerVelocity.y = -400.f;
                gsUpdater.setPlayerVelocity(player, playerVelocity);
            }
        }
        if(projectile && player.getProjectileCooldown() == 0){
            int projId = gameState.getProjectileIds();
            gsUpdater.setProjectileIds(projId + 1);

            Projectile proj(projId, {20.f,20.f}, player.getPosition());
            proj.setSpeed(proj.getSpeed() * (player.getFacing() == Player::FACING_RIGHT ? 1 : -1));
            gsUpdater.addProjectile(proj);
            gsUpdater.setPlayerProjectileCooldown(player, 100);
        }
        return true;
    }
    playerInput(){};
};
struct playerInputWithId : public UpdateInfo{
    OBJECT_ID_TYPE playerId;
    struct playerInput playerInput;

    virtual bool applyUpdate(GameStateUpdater& gsUpdater, GameState & gameState) override{
        std::cout << "debug: applying user input " << playerId << "\n";
        try
        {
            Player &player = gameState.getPlayer(playerId);
            playerInput.applyUpdate(player, gsUpdater, gameState);
        }
        catch(const std::exception& e)
        {
            return false;
        }
        return true;
    }
    playerInputWithId(){}
    playerInputWithId(OBJECT_ID_TYPE playerId, struct playerInput playerInput)
        : playerId(playerId), playerInput(playerInput){}
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
