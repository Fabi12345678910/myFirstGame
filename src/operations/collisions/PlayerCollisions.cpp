#include "Collisions/PlayerCollisions.h"
#include "Stage.h"

void handlePlayerSolidCollision(GameStateUpdater& gsUpdater, Player& player, GameObject& object, sf::RectangleShape collisionPosition, sf::Vector2f playerVelocity);
void handlePlayerSemiSolidCollision(GameStateUpdater& gsUpdater, Player& player, GameObject& object, sf::RectangleShape collisionPosition, sf::Vector2f playerVelocity);
void handlePlayerHazardCollision(GameStateUpdater& gsUpdater, Player& player, GameObject& object, sf::RectangleShape collisionPosition, sf::Vector2f playerVelocity, Stage const & stage);
void handlePlayerJumpPadCollision(GameStateUpdater& gsUpdater, Player& player, GameObject& object, sf::RectangleShape collisionPosition, sf::Vector2f playerVelocity);

void handlePlayerCollision(GameStateUpdater& gsUpdater, Player& player, Collidable& object, sf::RectangleShape collisionPosition, sf::Vector2f playerVelocity, GameState& gameState){
    GameObject& gameObject = dynamic_cast<GameObject&>(object);
    switch (object.collisionKind())
    {
    case CollisionKind::None:
        break;
    case CollisionKind::Solid:
        handlePlayerSolidCollision(gsUpdater, player, gameObject, collisionPosition, playerVelocity);
        break;
    case CollisionKind::SemiSolid:
        handlePlayerSemiSolidCollision(gsUpdater, player, gameObject, collisionPosition, playerVelocity);
        break;
    case CollisionKind::Hazard:
        handlePlayerHazardCollision(gsUpdater, player, gameObject, collisionPosition, playerVelocity, gameState.getStage());
        break;
    case CollisionKind::JumpPad:
        handlePlayerJumpPadCollision(gsUpdater, player, gameObject, collisionPosition, playerVelocity);
        break;
    default:
        break;
    }
}
void handlePlayerSolidCollision(GameStateUpdater& gsUpdater, Player& player, GameObject& object, sf::RectangleShape collisionPosition, sf::Vector2f playerVelocity){
    sf::FloatRect playerBounds = collisionPosition.getGlobalBounds();
    sf::FloatRect otherBounds = object.getShape().getGlobalBounds();

    float dx = (playerBounds.position.x + playerBounds.size.x / 2.f) 
         - (otherBounds.position.x + otherBounds.size.x / 2.f);

    float dy = (playerBounds.position.y + playerBounds.size.y / 2.f) 
         - (otherBounds.position.y + otherBounds.size.y / 2.f);

    float combinedHalfWidths = (playerBounds.size.x / 2.f) + (otherBounds.size.x / 2.f);
    float combinedHalfHeights = (playerBounds.size.y / 2.f) + (otherBounds.size.y / 2.f);

    // Only handle if actually colliding
    float overlapX = combinedHalfWidths - abs(dx);
    float overlapY = combinedHalfHeights - abs(dy);

    //TODO: this needs rework, we have to check both collision types correctly, not skip one because the other was bigger
    if (overlapX < overlapY) {
        sf::Vector2f playerMovement = collisionPosition.getGlobalBounds().position - player.getShape().getGlobalBounds().position;
        // Horizontal collision
        if (dx > 0.f) {
            // Player is on the right
            // move Player to the left till the object
            playerMovement.x += overlapX;
            gsUpdater.deltaMovePlayer(player.getId(), sf::Vector2f(overlapX, 0.f));
//            player.getShape().move(sf::Vector2f(overlapX, 0.f));
            // std::cout << "Collision from left\n";
        } else {
            // Player is on the left
            playerMovement.x -= overlapX;
            gsUpdater.deltaMovePlayer(player.getId(), sf::Vector2f(-overlapX, 0.f));
            // std::cout << "Collision from right\n";
        }
        gsUpdater.deltaMovePlayer(player.getId(), playerMovement);
        playerVelocity.x = 0.f;
        gsUpdater.setPlayerVelocity(player.getId(), playerVelocity);
    } else {
        sf::Vector2f playerMovement = collisionPosition.getGlobalBounds().position - player.getShape().getGlobalBounds().position;
        // Vertical collision
        if (dy > 0.f) {
            // Player is below
            playerMovement.y += overlapY;
            // std::cout << "Collision from above\n";
        } else {
            // Player is above
            playerMovement.y -= overlapY;
            gsUpdater.setPlayerOnGround(player.getId(), true);
            // std::cout << "Collision from below / landed\n";
        }
        gsUpdater.deltaMovePlayer(player.getId(), playerMovement);
        playerVelocity.y = 0.f;
        gsUpdater.setPlayerVelocity(player.getId(), playerVelocity);
    }
}

static bool crossedTopFromAbove(const sf::RectangleShape oldPosition, sf::RectangleShape collisionPosition, const sf::FloatRect& platform) {
    const float prevPlayerBottomY = oldPosition.getGlobalBounds().position.x;
    const float platformTop = platform.position.y;
    const float nowBottom   = collisionPosition.getGlobalBounds().position.x + collisionPosition.getGlobalBounds().size.y;
    return (prevPlayerBottomY <= platformTop) && (nowBottom >= platformTop);
}

void handlePlayerSemiSolidCollision(GameStateUpdater& gsUpdater, Player& player, GameObject& object, sf::RectangleShape collisionPosition, sf::Vector2f playerVelocity){
    // One-way platform: only land when falling from above and crossing the top
    const sf::FloatRect tBox = object.getShape().getGlobalBounds();
    sf::Vector2f playerMovement = collisionPosition.getGlobalBounds().position - player.getShape().getGlobalBounds().position;

    const bool movingDown = playerVelocity.y > 0.f;
    if (movingDown && crossedTopFromAbove(player.getShape(), collisionPosition, tBox)) {
        const float platformTop = tBox.position.y;
        const float correction  = platformTop - collisionPosition.getGlobalBounds().position.y - collisionPosition.getGlobalBounds().size.y;
        playerMovement.y+=correction;
        gsUpdater.deltaMovePlayer(player.getId(), playerMovement);
        playerVelocity.y = 0.f;
        player.setVelocity(playerVelocity);
        gsUpdater.setPlayerVelocity(player.getId(), playerVelocity);
        gsUpdater.setPlayerOnGround(player.getId(), true);
    }else{

    }
}
void handlePlayerHazardCollision(GameStateUpdater& gsUpdater, Player& player, GameObject& object, sf::RectangleShape collisionPosition, sf::Vector2f playerVelocity, Stage const & stage){
    const auto& spawns = stage.getSpawnPoints();
    if (!spawns.empty()) {
        gsUpdater.absoluteMovePlayer(player.getId(), spawns.front());
    }
    gsUpdater.setPlayerVelocity(player.getId(), sf::Vector2f{0.f, 0.f});

}
void handlePlayerJumpPadCollision(GameStateUpdater& gsUpdater, Player& player, GameObject& object, sf::RectangleShape collisionPosition, sf::Vector2f playerVelocity){
    //const sf::FloatRect pNow = player.getShape().getGlobalBounds();
    const sf::FloatRect tBox = object.getShape().getGlobalBounds();
    const bool movingDown = playerVelocity.y > 0.f;

    if (movingDown && crossedTopFromAbove(player.getShape(), collisionPosition, tBox)) {
        // Tune this impulse to taste; gravity is positive downward
        constexpr float kJumpPadImpulse = -700.f; // upward
        playerVelocity.y = kJumpPadImpulse;
        gsUpdater.setPlayerVelocity(player.getId(), playerVelocity);
    }
}