#include "GameUpdate.h"
#include <iostream>
#include "StageObject.h"
#include "Projectile.h"
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
            playerVelocity.x = -player.getSpeed();
            gsUpdater.setPlayerVelocity(gameState.getPlayer(input.playerId), playerVelocity);
            gsUpdater.setPlayerFacing(gameState.getPlayer(input.playerId), Player::FACING_LEFT);
        }
        if(input.playerInput.moveRight){
            playerVelocity.x = player.getSpeed();
            gsUpdater.setPlayerVelocity(gameState.getPlayer(input.playerId), playerVelocity);
            gsUpdater.setPlayerFacing(gameState.getPlayer(input.playerId), Player::FACING_RIGHT);
        }
        if(!input.playerInput.moveLeft&&!input.playerInput.moveRight){
            playerVelocity.x = 0;
            gsUpdater.setPlayerVelocity(gameState.getPlayer(input.playerId), playerVelocity);
        }
        if(input.playerInput.jump){
            if(player.getIsOnGround()){
                playerVelocity.y = -400.f;
                gsUpdater.setPlayerVelocity(gameState.getPlayer(input.playerId), playerVelocity);
            }
        }
        if(input.playerInput.projectile && player.getProjectileCooldown() == 0){
            int projId = gameState.getProjectileIds();
            gsUpdater.setProjectileIds(projId + 1);

            Projectile proj(projId, {20.f,20.f}, player.getPosition());
            proj.setSpeed(proj.getSpeed() * (player.getFacing() == Player::FACING_RIGHT ? 1 : -1));
            gsUpdater.addProjectile(proj);
            gsUpdater.setPlayerProjectileCooldown(player, 100);
        }
    }

    //move all movable objects
    for(Player& player : gameState.getPlayers()){

        if(player.getProjectileCooldown() > 0){
            gsUpdater.setPlayerProjectileCooldown(player, player.getProjectileCooldown() - 1);
        }

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
    for (Projectile& projectile : gameState.getProjectiles()) {
        if (projectile.getIsActive()) {
            projectile.getShape().move(sf::Vector2f(projectile.getSpeed(), 0) * deltaTime);
            // check collision with players
            for (Player &player: gameState.getPlayers()){
            
                const Collidable *collidable = dynamic_cast<const Collidable*>(&player);
                if(collidable != NULL){
                    if (projectile.getShape().getGlobalBounds().findIntersection(player.getShape().getGlobalBounds())) {
                        //std::cout << "detected collision\n";

                    }
                }
                else { std::cout << "projectile collided with a non collidable player"; } 
            }
            // check collision with stage objects
            for (StageObject &stageObject: gameState.getStage().getStageObjects()){

            const Collidable *collidable = dynamic_cast<const Collidable*>(&stageObject);
            if(collidable != NULL){

                if (projectile.getShape().getGlobalBounds().findIntersection(stageObject.getShape().getGlobalBounds())) {
                    //std::cout << "detected collision\n";
                    projectile.setIsActive(false);
                }
            }
        }
        }
    }
}
