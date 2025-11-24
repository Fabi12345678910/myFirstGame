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
};
