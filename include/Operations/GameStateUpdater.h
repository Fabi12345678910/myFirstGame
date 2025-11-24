#pragma once
#include "Player.h"
//handles calculated updates for a specific gamestate, may be implemented different on client/server
//all methods required by either the client or the server should be defined here and used in Gameupdate.update()
class GameStateUpdater
{
private:
    /* data */
public:
    GameStateUpdater(/* args */);
    ~GameStateUpdater();
    virtual void setPlayerVelocity(OBJECT_ID_TYPE playerId, sf::Vector2f velocity) = 0;
    virtual void deltaMovePlayer(OBJECT_ID_TYPE playerId, sf::Vector2f deltaPosition) = 0;
    virtual void absoluteMovePlayer(OBJECT_ID_TYPE playerId, sf::Vector2f absolutePosition) = 0;
    virtual void setPlayerOnGround(OBJECT_ID_TYPE playerId, bool isOnGround) = 0;
};

GameStateUpdater::GameStateUpdater(/* args */)
{
}

GameStateUpdater::~GameStateUpdater()
{
}
