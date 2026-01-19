#pragma once

#include "GameStateUpdater.h"
#include "GameState.h"
#include "Projectile.h"
#include "plog/Log.h"
#include <set>
class ClientGameStateUpdater: public GameStateUpdater
{
private:
    OBJECT_ID_TYPE localPlayerId;
    Player& localPlayer;
    bool localPlayerAdded = false;
    GameState& gameState;
    std::set<OBJECT_ID_TYPE> projectilesToDelete;
    std::set<OBJECT_ID_TYPE> playersToDelete;
public:
    void setGameState(GameState& gameState){
        this->gameState  = gameState;
    }
    ClientGameStateUpdater(GameState& gameState, OBJECT_ID_TYPE localPlayerId, Player& localPlayer):gameState(gameState), localPlayerId(localPlayerId), localPlayer(localPlayer){};
    ~ClientGameStateUpdater(){};
    virtual void setPlayerVelocity(Player& player, sf::Vector2f velocity) override {player.setVelocity(velocity);};
    virtual void deltaMovePlayer(Player& player, sf::Vector2f deltaPosition) override {player.getShape().move(deltaPosition);};
    virtual void absoluteMovePlayer(Player& player, sf::Vector2f absolutePosition) override {player.setPosition(absolutePosition);};
    virtual void setPlayerOnGround(Player& player, bool isOnGround) override {player.setIsOnGround(isOnGround);};
    virtual void setPlayerProjectileCooldown(Player& player, int16_t cooldown) override {player.setProjectileCooldown(cooldown);};
    virtual void setPlayerFacing(Player& player, bool facing) override {player.setFacing(facing);};
    virtual void addProjectile(Projectile& projectile) override {gameState.addProjectile(projectile);};
    virtual void setProjectileIds(OBJECT_ID_TYPE id) override {gameState.setProjectileIds(id);};
    virtual void addPlayer(Player& player) override {
        gameState.addPlayer(player);
        if(player.getId() == localPlayerId){
            localPlayer = player;
            localPlayerAdded = true;
            PLOG_VERBOSE << "local player was added";
        }
    };
    bool isLocalPlayerAdded(){
        return localPlayerAdded;
    }
    virtual void projectileHitPlayer(Projectile& projectile, Player& player) override {};
    virtual void setReadyToPlay(Player& player, bool ready) override {};
    virtual void registerRemoveProjectile(Projectile& projectile) override{
        projectilesToDelete.insert(projectile.getId());
    }
    virtual void removeRegisteredProjectiles() override{
        for(const auto& projectileId: projectilesToDelete){
            gameState.removeProjectile(projectileId);
        }
        projectilesToDelete.clear();
    }
    virtual void registerRemovePlayer(Player& player) override{
        playersToDelete.insert(player.getId());
    };
    virtual void removeRegisteredPlayers() override{
        for(const auto& playerId: playersToDelete){
            gameState.removePlayer(playerId);
        }
        playersToDelete.clear();
    };
};
