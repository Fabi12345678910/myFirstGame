#include "GameUpdate.h"
#include <iostream>
#include "StageObject.h"
#include "Collidable.h"
#include "Collisions/PlayerCollisions.h"
#include "Operations/GameStateUpdater.h"
#include "Inputs.h"

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

//
void updateGame(GameStateUpdater& gsUpdater, std::vector<playerInputWithId> inputs, GameState& gameState, float deltaTime){
    //handleInputs
    for(auto& input: inputs){
        Player &player = gameState.getPlayer(input.playerId);
        sf::Vector2f playerVelocity = player.getVelocity();
        if(input.playerInput.moveLeft){
            playerVelocity.x -= player.getSpeed();
            gsUpdater.setPlayerVelocity(input.playerId, playerVelocity);
        }
        if(input.playerInput.moveRight){
            playerVelocity.x = player.getSpeed();
            gsUpdater.setPlayerVelocity(input.playerId, playerVelocity);
        }
        if(input.playerInput.jump){
            if(player.getIsOnGround()){
                playerVelocity.y = -400.f;
                gsUpdater.setPlayerVelocity(input.playerId, playerVelocity);
            }
        }
    }

    //move all movable objects
    for(Player& player : gameState.getPlayers()){
        //set player not on ground unless otherwise computed by a collision later
        gsUpdater.setPlayerOnGround(player.getId(), false);

        const sf::FloatRect before = player.getShape().getGlobalBounds();
        const float prevBottomY = before.position.y + before.size.y;

        sf::Vector2f playerVelocity = player.getVelocity();
        playerVelocity.y += player.getGravity() * deltaTime;
//        if (!player.getIsOnGround()) {
//            playerVelocity.y += player.getGravity() * deltaTime;
//        } else {
//            playerVelocity.y = 0.f;
//        }
        
//        player.setVelocity(playerVelocity);

        //std::cout << "player has velocity " << player.getVelocity().x << ',' << player.getVelocity().y << '\n';
        sf::RectangleShape newPosition(player.getShape());
        newPosition.move(player.getVelocity() * deltaTime);
//        player.getShape().move(player.getVelocity() * deltaTime);

        for (StageObject &stageObject: gameState.getStage().getStageObjects()){

            const Collidable *collidable = dynamic_cast<const Collidable*>(&stageObject);
            if(collidable != NULL){
                if (newPosition.getGlobalBounds().findIntersection(stageObject.getShape().getGlobalBounds())) {
                    //std::cout << "detected collision\n";
                    handlePlayerCollision(gsUpdater, player, stageObject, newPosition, playerVelocity, gameState);
                }
            }
        }
        applyWrapEdgesX(player, gameState.getStage());
        applyVoidTeleportY(player, gameState.getStage());
    }
}
/*
void updateGame(GameState& gameState, float deltaTime){
    for(Player& player : gameState.getPlayers()){

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
}*/
