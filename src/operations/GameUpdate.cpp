#include "GameUpdate.h"
#include "StageObject.h"
#include "Projectile.h"
#include "Collidable.h"
#include "Collisions/PlayerCollisions.h"
#include "Operations/GameStateUpdater.h"
#include "plog/Log.h"
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

void updateGame(GameStateUpdater& gsUpdater, GameState& gameState, float deltaTime, std::vector<Player*> playersToUpdate, std::vector<Projectile*> projectilesToUpdate){

    //move all movable objects
    for(Player* player : playersToUpdate){

        // Dead players should not participate in physics/collisions.
        if (player->getHealth() <= 0) {
            continue;
        }

        if(player->getProjectileCooldown() > 0){
            gsUpdater.setPlayerProjectileCooldown(*player, player->getProjectileCooldown() - 1);
        }

        //set player not on ground unless otherwise computed by a collision later
        gsUpdater.setPlayerOnGround(*player, false);

        const sf::FloatRect before = player->getShape().getGlobalBounds();
        const float prevBottomY = before.position.y + before.size.y;

        sf::Vector2f playerVelocity = player->getVelocity();
        playerVelocity.y += player->getGravity() * deltaTime;
        sf::RectangleShape newPosition(player->getShape());
        newPosition.move(player->getVelocity() * deltaTime);

        bool movementHandledByCollision = false;
        for (StageObject &stageObject: gameState.getStage().getStageObjects()){

            const Collidable *collidable = dynamic_cast<const Collidable*>(&stageObject);
            if(collidable != NULL){
                if (newPosition.getGlobalBounds().findIntersection(stageObject.getShape().getGlobalBounds())) {
                    if(handlePlayerCollision(gsUpdater, *player, stageObject, newPosition, playerVelocity, gameState)){
                        movementHandledByCollision = true;
                    };
                }
            }
        }
        if(applyWrapEdgesX(gsUpdater, *player, newPosition, playerVelocity, gameState.getStage())){
            movementHandledByCollision = true;
        }
        if(applyVoidTeleportY(gsUpdater, *player, newPosition, playerVelocity, gameState.getStage())){
            movementHandledByCollision = true;
        }
        if(!movementHandledByCollision){
            gsUpdater.absoluteMovePlayer(*player, newPosition.getPosition());
            gsUpdater.setPlayerVelocity(*player, playerVelocity);
        }
    }
    for (Projectile* projectile : projectilesToUpdate) {
        projectile->getShape().move(sf::Vector2f(projectile->getSpeed(), 0) * deltaTime);
        // check collision with players
        for (auto itPlayer = gameState.getPlayersBegin(); itPlayer != gameState.getPlayersEnd(); itPlayer++){
            Player& player = itPlayer->second;
            if (player.getHealth() <= 0) {
                continue;
            }
        
            const Collidable *collidable = dynamic_cast<const Collidable*>(&player);
            if(collidable != NULL){
                if (projectile->getShape().getGlobalBounds().findIntersection(player.getShape().getGlobalBounds())) {
                    gsUpdater.registerRemoveProjectile(*projectile);
                    gsUpdater.projectileHitPlayer(*projectile, player);
                }
            }
            else { PLOG_WARNING << "projectile collided with a non collidable player"; } 
        }
        // check collision with stage objects
        for (StageObject &stageObject: gameState.getStage().getStageObjects()){

            const Collidable *collidable = dynamic_cast<const Collidable*>(&stageObject);
            if(collidable != NULL){

                if (projectile->getShape().getGlobalBounds().findIntersection(stageObject.getShape().getGlobalBounds())) {
                    gsUpdater.registerRemoveProjectile(*projectile);
                }
            }
        }
    }
}

void updateGame(GameStateUpdater& gsUpdater, GameState& gameState, float deltaTime){
    std::vector<Player*> playersToUpdate;
    std::vector<Projectile*> projectilesToUpdate;
    for (auto itPlayer = gameState.getPlayersBegin(); itPlayer != gameState.getPlayersEnd(); itPlayer++)
    {
        if(!itPlayer->second.getIsGhostPlayer()){
            playersToUpdate.push_back(&itPlayer->second);
        }
    }
    for (auto itProjectile = gameState.getProjectilesBegin(); itProjectile != gameState.getProjectilesEnd(); itProjectile++){
        projectilesToUpdate.push_back(&itProjectile->second);
    }
    
    updateGame(gsUpdater, gameState, deltaTime, playersToUpdate, projectilesToUpdate);
}

void updateGameSinglePlayer(GameStateUpdater& gsUpdater, GameState& gameState, Player& player, float deltaTime){
    std::vector<Player*> playersToUpdate = {&player};
    std::vector<Projectile*> projectilesToUpdate;
    
    updateGame(gsUpdater, gameState, deltaTime, playersToUpdate, projectilesToUpdate);
}