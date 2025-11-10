#include <collision/PlayerCollidableVisitor.h>
#include <collision/Collidable.h>
#include "Player.h"
#include "StageObject.h"
#include <iostream>

PlayerCollidableVisitor::PlayerCollidableVisitor(Player& p) : player(p) {}

void PlayerCollidableVisitor::visit(Collidable& c) {
    Collidable::secureReverseCollision(player, c);
}

void PlayerCollidableVisitor::visit(Player& player2) {
    std::cout << "player collided with player\n";
}

void visit(StageObject& stageObject){
    //std::cout << "player collided with platform\n";
    const StageObjectType type = stageObject.getType();
    const sf::FloatRect stageObjectBounds = stageObject.getShape().getGlobalBounds();
    const sf::FloatRect playerBounds = player.getShape().getGlobalBounds();
    const float prevBottomY = player.getPrevBottomY(); // store previous bottom y somewhere
    switch (type) {
        case StageObjectType::Solid:
            Collidable::secureReverseCollision(player, tile);
            break;
        case StageObjectType::SemiSolid:
            if (player.getVelocity().y > 0.f && crossedTopFromAbove(playerNow, prevBottomY, tile.getShape().getGlobalBounds())) {
                // land on semi-solid platform
                float correction = tile.getShape().getGlobalBounds().position.y - playerNow.position.y - playerNow.size.y;
                player.getShape().move(sf::Vector2f{0.f, correction});
                auto v = player.getVelocity();
                v.y = 0.f;
                player.setVelocity(v);
                player.setIsOnGround(true);
            }
            break;
        case StageObjectType::Hazard:
            respawnToFirstSpawn(player, stage);
            break;
        case StageObjectType::JumpPad:
            if (player.getVelocity().y > 0.f && crossedTopFromAbove(playerNow, prevBottomY, tile.getShape().getGlobalBounds())) {
                constexpr float kJumpPadImpulse = -700.f;
                auto v = player.getVelocity();
                v.y = kJumpPadImpulse;
                player.setVelocity(v);
                player.setIsOnGround(false);
            }
            break;
        default: break;
    }
}
