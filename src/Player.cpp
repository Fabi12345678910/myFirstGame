#include "../include/Player.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        velocity.x -= speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        velocity.x += speed;

    if (!isOnGround) {
        velocity.y += gravity * dt;
    } else {
        velocity.y = 0.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && isOnGround) {
        velocity.y = -400.f;
        isOnGround = false;
    }

    shape.move(velocity * dt);
    isOnGround = false;
}

void Player::onCollision(StaticCollidable& other) {
    sf::FloatRect playerBounds = getBounds();
    sf::FloatRect otherBounds = other.getBounds();

    float dx = (playerBounds.position.x + playerBounds.size.x / 2.f) - (otherBounds.position.x + otherBounds.position.x / 2.f);
    float dy = (playerBounds.position.y + playerBounds.size.y / 2.f) - (otherBounds.position.y + otherBounds.size.y / 2.f);

    float combinedHalfWidths = (playerBounds.position.x / 2.f) + (otherBounds.position.x / 2.f);
    float combinedHalfHeights = (playerBounds.size.y / 2.f) + (otherBounds.size.y / 2.f);

    // Only handle if actually colliding
    float overlapX = combinedHalfWidths - abs(dx);
    float overlapY = combinedHalfHeights - abs(dy);

    if (overlapX < overlapY) {
        // Horizontal collision
        if (dx > 0.f) {
            // Player is on the right
            shape.move(sf::Vector2f(overlapX, 0.f));
            cout << "Collision from left\n";
        } else {
            // Player is on the left
            shape.move(sf::Vector2f(-overlapX, 0.f));
            cout << "Collision from right\n";
        }
        velocity.x = 0.f;
    } else {
        // Vertical collision
        if (dy > 0.f) {
            // Player is below
            shape.move(sf::Vector2f(0.f, overlapY));
            velocity.y = 0.f;
            cout << "Collision from above\n";
        } else {
            // Player is above
            shape.move(sf::Vector2f(0.f, -overlapY));
            velocity.y = 0.f;
            isOnGround = true;
            cout << "Collision from below / landed\n";
        }
    }
}

void Player::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}