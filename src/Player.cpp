#include "../include/Player.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <cmath>
using namespace std;

Player::Player(float width, float height, float x, float y) {
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(sf::Color::Magenta);
    shape.setPosition(sf::Vector2f(x, y));
    prevPosition = shape.getPosition();
    velocity = sf::Vector2f(0.f, 0.f);
}

sf::FloatRect Player::getBounds() const {
    return shape.getGlobalBounds();
}

void Player::update(float dt) {
    // Keep last frame position (useful for landing detection / one-way later)
    prevPosition = shape.getPosition();
    // Horizontal input
    velocity.x = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        velocity.x -= speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        velocity.x += speed;
    // Gravity
    if (!isOnGround) {
        velocity.y += gravity * dt;
    } else {
        // allow walking off edges; don't pin vy to zero forever
        // vy will immediately be affected by gravity next frame if no ground
        velocity.y = 0.f;
    }
    // Jump
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && isOnGround) {
        velocity.y = -400.f;
        isOnGround = false;
    }
    // Integrate
    shape.move(velocity * dt);
    // Reset ground; collision will set it back if we landed this frame
    isOnGround = false;
}

void Player::onCollision(StaticCollidable& other) {
    const sf::FloatRect a = getBounds();
    const sf::FloatRect b = other.getBounds();

    // Centers
    const float aCx = a.position.x + a.size.x * 0.5f;
    const float aCy = a.position.y + a.size.y * 0.5f;
    const float bCx = b.position.x + b.size.x * 0.5f;
    const float bCy = b.position.y + b.size.y * 0.5f;

    // Deltas
    const float dx = aCx - bCx;
    const float dy = aCy - bCy;

    // Combined half-extents
    const float combinedHalfW = (a.size.x * 0.5f) + (b.size.x * 0.5f);
    const float combinedHalfH = (a.size.y * 0.5f) + (b.size.y * 0.5f);

    // Overlaps (positive means penetration)
    const float overlapX = combinedHalfW - std::fabs(dx);
    const float overlapY = combinedHalfH - std::fabs(dy);

    if (overlapX <= 0.f || overlapY <= 0.f) return;

    // ---------- ONE-WAY PLATFORM HANDLING ----------
    if (other.getType() == TileType::OneWay) {
        // We only collide when falling onto the top surface.
        const float prevBottom = prevPosition.y + a.size.y;
        const float otherTop   = b.position.y;

        // Conditions to ALLOW collision:
        //  - we were above the platform top last frame (prevBottom <= otherTop)
        //  - and we are moving downward (velocity.y >= 0)
        const bool allow = (prevBottom <= otherTop) && (velocity.y >= 0.f);

        if (!allow) {
            // ignore this collision completely (pass through)
            return;
        }

        // Snap on top (vertical resolve only)
        const float penetration = (a.position.y + a.size.y) - otherTop;
        shape.move({ 0.f, -penetration });
        velocity.y = 0.f;
        isOnGround = true;
        return;
    }
    // ---------- END ONE-WAY HANDLING ----------

    // Normal solids: resolve along least penetration
    if (overlapX < overlapY) {
        // Horizontal resolution
        if (dx > 0.f) {
            shape.move({ overlapX, 0.f });
        } else {
            shape.move({ -overlapX, 0.f });
        }
        velocity.x = 0.f;
    } else {
        // Vertical resolution
        if (dy > 0.f) {
            // Player center is below -> push down
            shape.move({ 0.f, overlapY });
            velocity.y = 0.f;
        } else {
            // Player center is above -> push up (land)
            shape.move({ 0.f, -overlapY });
            velocity.y = 0.f;

            // landed if we were going down (or stationary) this frame
            if (shape.getPosition().y >= prevPosition.y) {
                isOnGround = true;
            }
        }
    }
}

void Player::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

void Player::setPosition(const sf::Vector2f& p) {
    shape.setPosition(p);
}

sf::Vector2f Player::getPosition() const {
    return shape.getPosition();
}

void Player::respawn(const sf::Vector2f& spawn) {
    setPosition(spawn);
    velocity = sf::Vector2f(0.f, 0.f);
    isOnGround = false; // drop again
}