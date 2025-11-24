#include "GameUpdate.h"
#include <iostream>
#include "StageObject.h"
#include "Collidable.h"
#include "Collisions/PlayerCollisions.h"
#include "Operations/GameStateUpdater.h"
//#include "Inputs.h"

static bool applyWrapEdgesX(GameStateUpdater& gsUpdater, Player& player, sf::RectangleShape newPosition, sf::Vector2f playerVelocity, const Stage& stage) {
    if (!stage.getWrapEdgesX()) return false;
    const sf::FloatRect& stageBounds = stage.getBounds();
    sf::FloatRect newPositionBounds = newPosition.getGlobalBounds();

    if (newPositionBounds.position.x + newPositionBounds.size.x < stageBounds.position.x) {
        gsUpdater.absoluteMovePlayer(player, sf::Vector2f{stageBounds.position.x + stageBounds.size.x - newPositionBounds.size.x, newPositionBounds.position.y});
        return true;
    } else if (newPositionBounds.position.x > stageBounds.position.x + stageBounds.size.x) {
        gsUpdater.absoluteMovePlayer(player, sf::Vector2f{ stageBounds.position.x, newPositionBounds.position.y });
        return true;
    }
    return false;
}

static bool applyVoidTeleportY(GameStateUpdater& gsUpdater, Player& player, sf::RectangleShape newPosition, sf::Vector2f playerVelocity, const Stage& stage) {
    if (!stage.getVoidTeleportY()) return false;
    const sf::FloatRect& stageBounds = stage.getBounds();
    sf::FloatRect newPositionBounds = newPosition.getGlobalBounds();

    if (newPositionBounds.position.y > stageBounds.position.y + stageBounds.size.y) {
        gsUpdater.absoluteMovePlayer(player, sf::Vector2f{newPositionBounds.position.x, stageBounds.position.y - newPositionBounds.size.y - 1.f });
        
        playerVelocity.y = 0.f;
        gsUpdater.setPlayerVelocity(player, playerVelocity);
        return true;
    }
    return false;
}

//
void updateGame(GameStateUpdater& gsUpdater, std::vector<playerInputWithId> inputs, GameState& gameState, float deltaTime){
    //handleInputs
    for(auto& input: inputs){
        Player &player = gameState.getPlayer(input.playerId);
        sf::Vector2f playerVelocity = player.getVelocity();
        if(input.playerInput.moveLeft){
            playerVelocity.x -= player.getSpeed();
            gsUpdater.setPlayerVelocity(gameState.getPlayer(input.playerId), playerVelocity);
        }
        if(input.playerInput.moveRight){
            playerVelocity.x = player.getSpeed();
            gsUpdater.setPlayerVelocity(gameState.getPlayer(input.playerId), playerVelocity);
        }
        if(input.playerInput.jump){
            if(player.getIsOnGround()){
                playerVelocity.y = -400.f;
                gsUpdater.setPlayerVelocity(gameState.getPlayer(input.playerId), playerVelocity);
            }
        }
    }

    //move all movable objects
    for(Player& player : gameState.getPlayers()){
        //set player not on ground unless otherwise computed by a collision later
        gsUpdater.setPlayerOnGround(player, false);

        const sf::FloatRect before = player.getShape().getGlobalBounds();
        const float prevBottomY = before.position.y + before.size.y;

        sf::Vector2f playerVelocity = player.getVelocity();
        playerVelocity.y += player.getGravity() * deltaTime;
        sf::RectangleShape newPosition(player.getShape());
        newPosition.move(player.getVelocity() * deltaTime);

        bool movementHandledByCollision = false;
        for (StageObject &stageObject: gameState.getStage().getStageObjects()){

            const Collidable *collidable = dynamic_cast<const Collidable*>(&stageObject);
            if(collidable != NULL){
                if (newPosition.getGlobalBounds().findIntersection(stageObject.getShape().getGlobalBounds())) {
                    //std::cout << "detected collision\n";
                    if(handlePlayerCollision(gsUpdater, player, stageObject, newPosition, playerVelocity, gameState)){
                        movementHandledByCollision = true;
                    };
                }
            }
        }
        if(applyWrapEdgesX(gsUpdater, player, newPosition, playerVelocity, gameState.getStage())){
            movementHandledByCollision = true;
        }
        if(applyVoidTeleportY(gsUpdater, player, newPosition, playerVelocity, gameState.getStage())){
            movementHandledByCollision = true;
        }
        if(!movementHandledByCollision){
//            std::cout << "moving player because he did not already got handled\n";
            gsUpdater.absoluteMovePlayer(player, newPosition.getPosition());
            gsUpdater.setPlayerVelocity(player, playerVelocity);
        }
    }
}
