#include "Player.h"
#include <SFML/Graphics.hpp>

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
}

void Player::onCollision(StaticCollidable& other) {
    sf::FloatRect playerBounds = getBounds();
    sf::FloatRect otherBounds = other.getBounds();

    // Check if the collision is from above (landing on top)
    if (playerBounds.top + playerBounds.height <= otherBounds.top + velocity.y) {
        // Snap player to top of collidable
        shape.setPosition(playerBounds.left, otherBounds.top - playerBounds.height);

        // Reset vertical velocity
        velocity.y = 0.f;

        // Player is on the ground
        isOnGround = true;
    }
}

void Player::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}