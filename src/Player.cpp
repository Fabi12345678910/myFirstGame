#include "Player.h"
#include "stage/Floor.h"
#include "SFML/Graphics.hpp"

Player::Player(float width, float height, float x, float y){
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(sf::Color::Magenta);
    shape.setPosition(sf::Vector2f(x, y));
}

sf::FloatRect Player::getBounds() const {
    return shape.getGlobalBounds();
}

void Player::update(float dt) {
    velocity = {0.f, 0.f};

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        velocity.x -= speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        velocity.x += speed;

    if (!isOnGround) {
        velocity.y += 600*(gravity * dt);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && isOnGround) {
        velocity.y = -200.f;
        isOnGround = false;
    }

    shape.move(velocity * dt);
}

void Player::onCollision(Collidable& other) {
    //other.onCollision(*this);
}

void Player::onCollision(Floor& floor){
    this->isOnGround = true;
}

void Player::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}