#include "GameStateUpdater.h"
#include "GameState.h"
class ServerGameStateUpdater: public GameStateUpdater
{
private:
    GameState& gameState;
public:
    ServerGameStateUpdater(GameState& gameState):gameState(gameState){};
    ~ServerGameStateUpdater(){};
    virtual void setPlayerVelocity(Player& player, sf::Vector2f velocity) override {player.setVelocity(velocity);};
    virtual void deltaMovePlayer(Player& player, sf::Vector2f deltaPosition) override {player.getShape().move(deltaPosition);};
    virtual void absoluteMovePlayer(Player& player, sf::Vector2f absolutePosition) override {player.setPosition(absolutePosition);};
    virtual void setPlayerOnGround(Player& player, bool isOnGround) override {player.setIsOnGround(isOnGround);};
    virtual void setPlayerProjectileCooldown(Player& player, int16_t cooldown) override {player.setProjectileCooldown(cooldown);};
    virtual void setPlayerFacing(Player& player, bool facing) override {player.setFacing(facing);};
    virtual void addProjectile(Projectile& projectile) override {gameState.addProjectile(projectile);};
    virtual void setProjectileIds(OBJECT_ID_TYPE id) override {gameState.setProjectileIds(id);};
    virtual void addPlayer(Player& player) override {gameState.addPlayer(player);};
    virtual void projectileHitPlayer(Projectile& projectile, Player& player) override {gameState.getPlayer(player.getId()).setHealth(0);};
    virtual void setReadyToPlay(Player& player, bool ready) override {player.setReadyToPlay(ready);};
};
