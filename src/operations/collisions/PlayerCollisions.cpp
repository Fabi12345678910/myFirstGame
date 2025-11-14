#include "Collisions/PlayerCollisions.hpp"
#include "Stage.h"

void handlePlayerSolidCollision(Player& player, GameObject& object);
void handlePlayerSemiSolidCollision(Player& player, GameObject& object, float prevPlayerBottomY);
void handlePlayerHazardCollision(Player& player, GameObject& object, Stage const & stage);
void handlePlayerJumpPadCollision(Player& player, GameObject& object, float prevPlayerBottomY);

void handlePlayerCollision(Player& player, Collidable& object, float prevPlayerBottomY, Stage const& stage){
    GameObject& gameObject = dynamic_cast<GameObject&>(object);
    switch (object.collisionKind())
    {
    case CollisionKind::None:
        break;
    case CollisionKind::Solid:
        handlePlayerSolidCollision(player, gameObject);
        break;
    case CollisionKind::SemiSolid:
        handlePlayerSemiSolidCollision(player, gameObject, prevPlayerBottomY);
        break;
    case CollisionKind::Hazard:
        handlePlayerHazardCollision(player, gameObject, stage);
        break;
    case CollisionKind::JumpPad:
        handlePlayerJumpPadCollision(player, gameObject, prevPlayerBottomY);
        break;
    default:
        break;
    }
}
void handlePlayerSolidCollision(Player& player, GameObject& object){
    sf::FloatRect playerBounds = player.getShape().getGlobalBounds();
    sf::FloatRect otherBounds = object.getShape().getGlobalBounds();
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

static bool crossedTopFromAbove(const sf::FloatRect& playerNow, float prevPlayerBottomY, const sf::FloatRect& platform) {
    const float platformTop = platform.position.y;
    const float nowBottom   = playerNow.position.y + playerNow.size.y;
    return (prevPlayerBottomY <= platformTop) && (nowBottom >= platformTop);
}

void handlePlayerSemiSolidCollision(Player& player, GameObject& object, float prevPlayerBottomY){
    // One-way platform: only land when falling from above and crossing the top
    const sf::FloatRect pNow = player.getShape().getGlobalBounds();
    const sf::FloatRect tBox = object.getShape().getGlobalBounds();

    const bool movingDown = player.getVelocity().y > 0.f;
    if (movingDown && crossedTopFromAbove(pNow, prevPlayerBottomY, tBox)) {
        const float platformTop = tBox.position.y;
        const float correction  = platformTop - pNow.position.y - pNow.size.y;
        player.getShape().move(sf::Vector2f{0.f, correction});
        auto v = player.getVelocity();
        v.y = 0.f;
        player.setVelocity(v);
        player.setIsOnGround(true);
    }
}
void handlePlayerHazardCollision(Player& player, GameObject& object, Stage const & stage){
    const auto& spawns = stage.getSpawnPoints();
    if (!spawns.empty()) {
        player.getShape().setPosition(spawns.front());
    }
    player.setVelocity(sf::Vector2f{0.f, 0.f});
    player.setIsOnGround(false);
}
void handlePlayerJumpPadCollision(Player& player, GameObject& object, float prevPlayerBottomY){
    const sf::FloatRect pNow = player.getShape().getGlobalBounds();
    const sf::FloatRect tBox = object.getShape().getGlobalBounds();
    const bool movingDown = player.getVelocity().y > 0.f;

    if (movingDown && crossedTopFromAbove(pNow, prevPlayerBottomY, tBox)) {
        // Tune this impulse to taste; gravity is positive downward
        constexpr float kJumpPadImpulse = -700.f; // upward
        auto v = player.getVelocity();
        v.y = kJumpPadImpulse;
        player.setVelocity(v);
        player.setIsOnGround(false);
    }
}