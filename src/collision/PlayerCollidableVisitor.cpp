#include <collision/PlayerCollidableVisitor.h>
#include <collision/Collidable.h>
#include <Player.h>
#include <StageObject.h>
#include <iostream>
#include <cmath>

void PlayerCollidableVisitor::visit(Collidable& c) {
    Collidable::secureReverseCollision(player, c);
}

void PlayerCollidableVisitor::visit(Player& player2) {
    std::cout << "player collided with player\n";
}

void PlayerCollidableVisitor::visit(StageObject& stageObject) {
    const StageObjectType type = stageObject.getType();
    const sf::FloatRect stageObjectBounds = stageObject.getShape().getGlobalBounds();
    const sf::FloatRect playerBounds = player.getShape().getGlobalBounds();

    const bool falling = player.getVelocity().y > 0.f;

    const float platformTop  = stageObjectBounds.position.y;
    const float playerBottom = playerBounds.position.y + playerBounds.size.y;
    const float penTop       = playerBottom - platformTop; // overlap from above

    switch (type) {
        case StageObjectType::Solid: {
            // Axis-aligned bounding box (AABB) resolution
            const sf::FloatRect pBox = player.getShape().getGlobalBounds();
            const sf::FloatRect oBox = stageObject.getShape().getGlobalBounds();
            sf::Vector2f v = player.getVelocity();

            const float dx = (pBox.position.x + pBox.size.x / 2.f)
                           - (oBox.position.x + oBox.size.x / 2.f);
            const float dy = (pBox.position.y + pBox.size.y / 2.f)
                           - (oBox.position.y + oBox.size.y / 2.f);

            const float combinedHalfWidths  = (pBox.size.x / 2.f) + (oBox.size.x / 2.f);
            const float combinedHalfHeights = (pBox.size.y / 2.f) + (oBox.size.y / 2.f);

            const float overlapX = combinedHalfWidths  - std::abs(dx);
            const float overlapY = combinedHalfHeights - std::abs(dy);

            if (overlapX > 0.f && overlapY > 0.f) {
                if (overlapX < overlapY) {
                    // Horizontal collision
                    player.getShape().move({dx > 0.f ? overlapX : -overlapX, 0.f});
                    v.x = 0.f;
                    player.setVelocity(v);
                } else {
                    // Vertical collision
                    player.getShape().move({0.f, dy > 0.f ? overlapY : -overlapY});
                    v.y = 0.f;
                    player.setVelocity(v);
                    if (dy < 0.f) player.setIsOnGround(true);
                }
            }
            break;
        }
        case StageObjectType::SemiSolid: {
            if (falling) {
                const float penetration = penTop;
                if (penetration > 0.f && penetration <= playerBounds.size.y * 0.5f) {
                    player.getShape().move({0.f, -penetration});
                    auto v = player.getVelocity();
                    v.y = 0.f;
                    player.setVelocity(v);
                    player.setIsOnGround(true);
                }
            }
            break;
        }
        case StageObjectType::JumpPad: {
            if (falling) {
                const float penetration = penTop;
                if (penetration > 0.f && penetration <= playerBounds.size.y * 0.5f) {
                    auto v = player.getVelocity();
                    v.y = -700.f; // bounce impulse
                    player.setVelocity(v);
                    player.setIsOnGround(false);
                }
            }
            break;
        }
        case StageObjectType::Hazard: {
            player.setVelocity({0.f, 0.f});
            player.setIsOnGround(false);
            player.setIsEliminated(true);
            break;
        }
        default:
            break;
    }
}