#include "../../include/stage/Floor.h"
#include "SFML/Graphics.hpp"

Floor::Floor(float width, float height, float x, float y){
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(sf::Color::Green);
    shape.setPosition(sf::Vector2f(x, y));
}

sf::FloatRect Floor::getBounds() const {
    return shape.getGlobalBounds();
}

void Floor::draw(sf::RenderWindow& window) const {
        window.draw(shape);
}

void Floor::onCollision(Collidable& other) {
    // maybe in the future
}