#include "Player.h"
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;

Player::Player(float width, float height, float x, float y){
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(sf::Color::Magenta);
    shape.setPosition(sf::Vector2f(x, y));
}

sf::FloatRect Player::getBounds() const {
    return shape.getGlobalBounds();
}

void Player::update(float dt) {
    velocity.x = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        velocity.x -= speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        velocity.x += speed;

    if (!isOnGround) {
        velocity.y += gravity * dt;
    } else {
        velocity.y = 0.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && isOnGround) {
        velocity.y = -400.f;
        isOnGround = false;
    }

    shape.move(velocity * dt);
    isOnGround = false;
}

void Player::onCollision(StaticCollidable& other) {
    sf::FloatRect playerBounds = getBounds();
    sf::FloatRect otherBounds = other.getBounds();

    float dx = (playerBounds.left + playerBounds.width / 2.f) - (otherBounds.left + otherBounds.width / 2.f);
    float dy = (playerBounds.top + playerBounds.height / 2.f) - (otherBounds.top + otherBounds.height / 2.f);

    float combinedHalfWidths = (playerBounds.width / 2.f) + (otherBounds.width / 2.f);
    float combinedHalfHeights = (playerBounds.height / 2.f) + (otherBounds.height / 2.f);

    // Only handle if actually colliding
    if (abs(dx) < combinedHalfWidths && abs(dy) < combinedHalfHeights) {
        float overlapX = combinedHalfWidths - abs(dx);
        float overlapY = combinedHalfHeights - abs(dy);

        if (overlapX < overlapY) {
            // Horizontal collision
            if (dx > 0.f) {
                // Player is on the right
                shape.move(overlapX, 0.f);
                cout << "Collision from left\n";
            } else {
                // Player is on the left
                shape.move(-overlapX, 0.f);
                cout << "Collision from right\n";
            }
            velocity.x = 0.f;
        } else {
            // Vertical collision
            if (dy > 0.f) {
                // Player is below
                shape.move(0.f, overlapY);
                velocity.y = 0.f;
                cout << "Collision from above\n";
            } else {
                // Player is above
                shape.move(0.f, -overlapY);
                velocity.y = 0.f;
                isOnGround = true;
                cout << "Collision from below / landed\n";
            }
        }
    }
}

void Player::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}