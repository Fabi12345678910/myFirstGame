#include "GameUpdate.h"
#include <iostream>
#include "StageObject.h"
#include <collision/Collidable.h>
#include <collision/PlayerCollidableVisitor.h>

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

        PlayerCollidableVisitor visitor(player);
        for (StageObject &stageObject: gameState.getStage().getStageObjects()){
            if (player.getShape().getGlobalBounds().findIntersection(stageObject.getShape().getGlobalBounds())) {
                stageObject.accept(visitor);
            }
        }
        // Void teleport Y: if player falls below the stage, wrap to above the top
        const auto& b = gameState.getStage().getBounds();
        const auto r = player.getShape().getGlobalBounds();
        if (r.position.y > b.position.y + b.size.y) {
            const float newY = b.position.y - r.size.y - 1.f;
            player.getShape().setPosition({ r.position.x, newY });
            auto v = player.getVelocity();
            v.y = 0.f;
            player.setVelocity(v);
            player.setIsOnGround(false);
        }
    }
}
