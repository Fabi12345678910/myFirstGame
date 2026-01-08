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
    bool readyToPlay = false;
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

            const sf::Vector2f bulletSize{20.f, 20.f};
            const sf::Vector2f playerPos = player.getPosition();
            const sf::Vector2f playerSize = player.getSize();
            const int dir = (player.getFacing() == Player::FACING_RIGHT) ? 1 : -1;
            const float margin = 10.f;
            sf::Vector2f spawnPos{
                (dir > 0) ? (playerPos.x + playerSize.x + margin)
                          : (playerPos.x - bulletSize.x - margin),
                playerPos.y + (playerSize.y - bulletSize.y) * 0.5f
            };

            Projectile proj(projId, bulletSize, spawnPos);
            proj.setSpeed(proj.getSpeed() * dir);
            gsUpdater.addProjectile(proj);
            gsUpdater.setPlayerProjectileCooldown(player, 100);
        }
        if(readyToPlay) {
            gsUpdater.setReadyToPlay(player, !player.getReadyToPlay());
        }
        return true;
    }
    playerInput(){};
};
struct playerInputWithId : public UpdateInfo{
    OBJECT_ID_TYPE playerId;
    struct playerInput playerInput;

    virtual bool applyUpdate(GameStateUpdater& gsUpdater, GameState & gameState) override{
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

struct indexedPlayerInput{
    TICK_TYPE idx;
    struct playerInput playerInput;
    indexedPlayerInput(){}
    indexedPlayerInput(TICK_TYPE idx, struct playerInput pInput) : idx(idx), playerInput(pInput) {}
};

struct indexedPlayerInputWithId{
    TICK_TYPE idx;
    struct playerInputWithId playerInputWithId;
    indexedPlayerInputWithId(){}
    indexedPlayerInputWithId(TICK_TYPE idx, struct playerInputWithId pInputWithId): idx(idx), playerInputWithId(pInputWithId){}
    indexedPlayerInputWithId(TICK_TYPE idx, struct playerInput pInput, OBJECT_ID_TYPE id) : 
        idx(idx), playerInputWithId(id, pInput){}
    indexedPlayerInputWithId(struct indexedPlayerInput idxPInput, OBJECT_ID_TYPE id) : idx(idxPInput.idx), playerInputWithId(id, idxPInput.playerInput){}
    void invalidateIdx(){idx = 0-1;};
    bool isIdxValid(){return idx != 0-1;}
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


inline sf::Packet& operator<<(sf::Packet& packet, const indexedPlayerInput& inputId) {
    return packet << inputId.idx << inputId.playerInput;
}

inline sf::Packet& operator>>(sf::Packet& packet, indexedPlayerInput& inputId) {
    return packet >> inputId.idx >> inputId.playerInput;
}
