#include "StageObject.h"
#include "SFML/Graphics.hpp"

StageObject::StageObject(float width, float height, float x, float y) {
    shape.setSize(sf::Vector2f(width, height));
    shape.setPosition(sf::Vector2f(x, y));
    shape.setFillColor(sf::Color::Green);
}

sf::FloatRect StageObject::getBounds() const {
    return shape.getGlobalBounds();
}

void StageObject::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}