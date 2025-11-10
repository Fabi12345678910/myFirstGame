#include "GameUpdate.h"
#include <iostream>
#include "StageObject.h"
#include <collision/Collidable.h>

void onCollision(Player& player, const GameObject& other);

static bool crossedTopFromAbove(const sf::FloatRect& playerNow, float prevBottomY, const sf::FloatRect& platform) {
    const float platformTop = platform.position.y;
    const float nowBottom   = playerNow.position.y + playerNow.size.y;
    return (prevBottomY <= platformTop) && (nowBottom >= platformTop);
}

static void respawnToFirstSpawn(Player& player, const Stage& stage) {
    const auto& spawns = stage.getSpawnPoints();
    if (!spawns.empty()) {
        player.getShape().setPosition(spawns.front());
    }
    player.setVelocity(sf::Vector2f{0.f, 0.f});
    player.setIsOnGround(false);
}

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


static void handleTileCollision(Player& player,
                                const StageObject& tile,
                                float prevBottomY,
                                const Stage& stage)
{
    switch (tile.getType()) {
        case StageObjectType::Solid: {
            onCollision(player, static_cast<const GameObject&>(tile));
            break;
        }
        case StageObjectType::SemiSolid: {
            // One-way platform: only land when falling from above and crossing the top
            const sf::FloatRect pNow = player.getShape().getGlobalBounds();
            const sf::FloatRect tBox = tile.getShape().getGlobalBounds();

            const bool movingDown = player.getVelocity().y > 0.f;
            if (movingDown && crossedTopFromAbove(pNow, prevBottomY, tBox)) {
                const float platformTop = tBox.position.y;
                const float correction  = platformTop - pNow.position.y - pNow.size.y;
                player.getShape().move(sf::Vector2f{0.f, correction});
                auto v = player.getVelocity();
                v.y = 0.f;
                player.setVelocity(v);
                player.setIsOnGround(true);
            }
            break;
        }
        case StageObjectType::Hazard: {
            respawnToFirstSpawn(player, stage);
            break;
        }
        case StageObjectType::JumpPad: {
            const sf::FloatRect pNow = player.getShape().getGlobalBounds();
            const sf::FloatRect tBox = tile.getShape().getGlobalBounds();
            const bool movingDown = player.getVelocity().y > 0.f;

            if (movingDown && crossedTopFromAbove(pNow, prevBottomY, tBox)) {
                // Tune this impulse to taste; gravity is positive downward
                constexpr float kJumpPadImpulse = -700.f; // upward
                auto v = player.getVelocity();
                v.y = kJumpPadImpulse;
                player.setVelocity(v);
                player.setIsOnGround(false);
            }
            break;
        }
        default:
            break;
    }
}

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
            // std::cout << "Collision from left\n";
        } else {
            // Player is on the left
            player.getShape().move(sf::Vector2f(-overlapX, 0.f));
            // std::cout << "Collision from right\n";
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
            // std::cout << "Collision from above\n";
        } else {
            // Player is above
            player.getShape().move(sf::Vector2f(0.f, -overlapY));
            playerVelocity.y = 0.f;
            player.setVelocity(playerVelocity);
            player.setIsOnGround(true);
            // std::cout << "Collision from below / landed\n";
        }
    }
}


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

        for (StageObject const &stageObject: gameState.getStage().getStageObjects()){

            const Collidable *collidable = dynamic_cast<const Collidable*>(&stageObject);
            if(collidable != NULL){

                if (player.getShape().getGlobalBounds().findIntersection(stageObject.getShape().getGlobalBounds())) {
                    //std::cout << "detected collision\n";
                    handleTileCollision(player, stageObject, prevBottomY, gameState.getStage());
                }
            }
        }
        applyWrapEdgesX(player, gameState.getStage());
        applyVoidTeleportY(player, gameState.getStage());
    }
}
