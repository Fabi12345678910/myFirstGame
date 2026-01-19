#pragma once

#include "GameStateUpdater.h"
#include "GameState.h"
#include <set>
class LocalPlayerGameStateUpdater: public GameStateUpdater
{
private:
    GameState& gameState;
    std::set<OBJECT_ID_TYPE> projectilesToDelete;

public:
    void setGameState(GameState& gameState){
        this->gameState  = gameState;
    }
    LocalPlayerGameStateUpdater(GameState& gameState):gameState(gameState){};
    ~LocalPlayerGameStateUpdater(){};
    virtual void setPlayerVelocity(Player& player, sf::Vector2f velocity) override {player.setVelocity(velocity);};
    virtual void deltaMovePlayer(Player& player, sf::Vector2f deltaPosition) override {player.getShape().move(deltaPosition);};
    virtual void absoluteMovePlayer(Player& player, sf::Vector2f absolutePosition) override {player.setPosition(absolutePosition);};
    virtual void setPlayerOnGround(Player& player, bool isOnGround) override {player.setIsOnGround(isOnGround);};
    virtual void setPlayerProjectileCooldown(Player& player, int16_t cooldown) override {player.setProjectileCooldown(cooldown);};
    virtual void setPlayerFacing(Player& player, bool facing) override {player.setFacing(facing);};
    virtual void addProjectile(Projectile& projectile) override {};
    virtual void setProjectileIds(OBJECT_ID_TYPE id) override {};
    virtual void addPlayer(Player& player) override {};
    virtual void projectileHitPlayer(Projectile& projectile, Player& player) override {};
    virtual void setReadyToPlay(Player& player, bool isReady) override {};
    virtual void registerRemoveProjectile(Projectile& projectile) override{
        projectilesToDelete.insert(projectile.getId());
    }
    virtual void removeRegisteredProjectiles() override{
        for(const auto& projectileId: projectilesToDelete){
            gameState.removeProjectile(projectileId);
        }
    }
};
