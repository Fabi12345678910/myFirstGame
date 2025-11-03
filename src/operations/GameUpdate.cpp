#include "GameUpdate.h"

#include <iostream>

void onCollision(Player& player, GameObject const & other) {
    sf::FloatRect playerBounds = player.getShape().getGlobalBounds();
    sf::FloatRect otherBounds = other.getShape().getGlobalBounds();
    sf::Vector2f playerVelocity = player.getVelocity();

    float dx = (playerBounds.position.x + playerBounds.size.x / 2.f) 
         - (otherBounds.position.x + otherBounds.size.x / 2.f);

    float dy = (playerBounds.position.y + playerBounds.size.y / 2.f) 
         - (otherBounds.position.y + otherBounds.size.y / 2.f);


    float combinedHalfWidths = (playerBounds.size.x / 2.f) + (otherBounds.size.x / 2.f);
    float combinedHalfHeights = (playerBounds.size.y / 2.f) + (otherBounds.size.y / 2.f);

    // Only handle if actually colliding
    float overlapX = combinedHalfWidths - abs(dx);
    float overlapY = combinedHalfHeights - abs(dy);

    if (overlapX < overlapY) {
        // Horizontal collision
        if (dx > 0.f) {
            // Player is on the right
            player.getShape().move(sf::Vector2f(overlapX, 0.f));
            std::cout << "Collision from left\n";
        } else {
            // Player is on the left
            player.getShape().move(sf::Vector2f(-overlapX, 0.f));
            std::cout << "Collision from right\n";
        }
        playerVelocity.x = 0.f;
        player.setVelocity(playerVelocity);
    } else {
        // Vertical collision
        if (dy > 0.f) {
            // Player is below
            player.getShape().move(sf::Vector2f(0.f, overlapY));
            playerVelocity.y = 0.f;
            player.setVelocity(playerVelocity);
            std::cout << "Collision from above\n";
        } else {
            // Player is above
            player.getShape().move(sf::Vector2f(0.f, -overlapY));
            playerVelocity.y = 0.f;
            player.setVelocity(playerVelocity);
            player.setIsOnGround(true);
            std::cout << "Collision from below / landed\n";
        }
    }
}

void updateGame(GameState& gameState, float deltaTime){
    for(Player& player : gameState.getPlayers()){

        player.setIsOnGround(false);

        sf::Vector2f playerVelocity = player.getVelocity();
        if (!player.getIsOnGround()) {
            playerVelocity.y += player.getGravity() * deltaTime;
        } else {
            playerVelocity.y = 0.f;
        }
        player.setVelocity(playerVelocity);

        std::cout << "player has velocity " << player.getVelocity().x << ',' <<player.getVelocity().y << '\n';
        player.getShape().move(player.getVelocity() * deltaTime);

        //somehow check all objects, idk how yet
        for (StageObject const &stageObject: gameState.getStage().getStageObjects()){

            const Collidable *collidable = dynamic_cast<const Collidable*>(&stageObject);
            if(collidable != NULL){

                if (player.getShape().getGlobalBounds().findIntersection(stageObject.getShape().getGlobalBounds())) {
                    std::cout << "detected collision\n";
                    onCollision(player, stageObject);
                }
            }
        }
    }
}