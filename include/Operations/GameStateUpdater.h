#pragma once
#include "Player.h"
//handles calculated updates for a specific gamestate, may be implemented different on client/server
//all methods required by either the client or the server should be defined here and used in Gameupdate.update()
class GameStateUpdater
{
private:
    /* data */
public:
    GameStateUpdater(/* args */){};
    ~GameStateUpdater(){};
    virtual void setPlayerVelocity(Player& player, sf::Vector2f velocity) = 0;
    virtual void deltaMovePlayer(Player& player, sf::Vector2f deltaPosition) = 0;
    virtual void absoluteMovePlayer(Player& player, sf::Vector2f absolutePosition) = 0;
    virtual void setPlayerOnGround(Player& player, bool isOnGround) = 0;
    virtual void setPlayerProjectileCooldown(Player& player, int16_t cooldown) = 0;
    virtual void setPlayerFacing(Player& player, bool facing) = 0;
    virtual void addProjectile(Projectile& projectile) = 0;
    virtual void setProjectileIds(OBJECT_ID_TYPE id) = 0;
    virtual void addPlayer(Player& player) = 0;
};