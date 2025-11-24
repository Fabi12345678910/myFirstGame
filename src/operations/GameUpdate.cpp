#include "GameUpdate.h"
#include <iostream>
#include "StageObject.h"
#include "Projectile.h"
#include "Collidable.h"
#include "Collisions/PlayerCollisions.hpp"

static void applyWrapEdgesX(Player& player, const Stage& stage) {
    if (!stage.getWrapEdgesX()) return;
    const auto& b = stage.getBounds();
    auto r = player.getShape().getGlobalBounds();

    if (r.position.x + r.size.x < b.position.x) {
        player.getShape().setPosition(sf::Vector2f{ b.position.x + b.size.x - r.size.x, r.position.y });
    }
    else if (r.position.x > b.position.x + b.size.x) {
        player.getShape().setPosition(sf::Vector2f{ b.position.x, r.position.y });
    }
}

static void applyVoidTeleportY(Player& player, const Stage& stage) {
    if (!stage.getVoidTeleportY()) return;
    const auto& b = stage.getBounds();
    auto r = player.getShape().getGlobalBounds();

    if (r.position.y > b.position.y + b.size.y) {
        const float newY = b.position.y - r.size.y - 1.f;
        player.getShape().setPosition(sf::Vector2f{ r.position.x, newY });
        auto v = player.getVelocity();
        v.y = 0.f;
        player.setVelocity(v);
        player.setIsOnGround(false);
    }
}

void updateGame(GameState& gameState, float deltaTime){    
    for(Player& player : gameState.getPlayers()){
        if (player.getCooldown() > 0) {
            player.setCooldown(player.getCooldown()-1); //shooting cooldown goes down
        }
        player.setIsOnGround(false);

        const sf::FloatRect before = player.getShape().getGlobalBounds();
        const float prevBottomY = before.position.y + before.size.y;

        sf::Vector2f playerVelocity = player.getVelocity();
        if (!player.getIsOnGround()) {
            playerVelocity.y += player.getGravity() * deltaTime;
        } else {
            playerVelocity.y = 0.f;
        }
        player.setVelocity(playerVelocity);

        //std::cout << "player has velocity " << player.getVelocity().x << ',' << player.getVelocity().y << '\n';
        player.getShape().move(player.getVelocity() * deltaTime);

        for (StageObject &stageObject: gameState.getStage().getStageObjects()){

            const Collidable *collidable = dynamic_cast<const Collidable*>(&stageObject);
            if(collidable != NULL){

                if (player.getShape().getGlobalBounds().findIntersection(stageObject.getShape().getGlobalBounds())) {
                    //std::cout << "detected collision\n";
                    handlePlayerCollision(player, stageObject, prevBottomY, gameState.getStage());
                }
            }
        }
        applyWrapEdgesX(player, gameState.getStage());
        applyVoidTeleportY(player, gameState.getStage());
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
