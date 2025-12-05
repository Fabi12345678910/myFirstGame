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
            sf::Vector2f playerVelocity = player.getVelocity();
            if(playerInput.moveLeft){
                playerVelocity.x = -player.getSpeed();
                gsUpdater.setPlayerVelocity(gameState.getPlayer(playerId), playerVelocity);
                gsUpdater.setPlayerFacing(gameState.getPlayer(playerId), Player::FACING_LEFT);
            }
            if(playerInput.moveRight){
                playerVelocity.x = player.getSpeed();
                gsUpdater.setPlayerVelocity(gameState.getPlayer(playerId), playerVelocity);
                gsUpdater.setPlayerFacing(gameState.getPlayer(playerId), Player::FACING_RIGHT);
            }
            if(!playerInput.moveLeft&&!playerInput.moveRight){
                playerVelocity.x = 0;
                gsUpdater.setPlayerVelocity(gameState.getPlayer(playerId), playerVelocity);
            }
            if(playerInput.jump){
                if(player.getIsOnGround()){
                    playerVelocity.y = -400.f;
                    gsUpdater.setPlayerVelocity(gameState.getPlayer(playerId), playerVelocity);
                }
            }
            if(playerInput.projectile && player.getProjectileCooldown() == 0){
                int projId = gameState.getProjectileIds();
                gsUpdater.setProjectileIds(projId + 1);

                Projectile proj(projId, {20.f,20.f}, player.getPosition());
                proj.setSpeed(proj.getSpeed() * (player.getFacing() == Player::FACING_RIGHT ? 1 : -1));
                gsUpdater.addProjectile(proj);
                gsUpdater.setPlayerProjectileCooldown(player, 100);
            }
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
