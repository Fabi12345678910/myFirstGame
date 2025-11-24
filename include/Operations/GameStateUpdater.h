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
};