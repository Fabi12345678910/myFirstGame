#include "StageObject.h"
#include "SFML/Graphics.hpp"

StageObject::StageObject(float width, float height, float x, float y, TileType type) {
    shape.setSize(sf::Vector2f(width, height));
    shape.setPosition(sf::Vector2f(x, y));
    // remember type
    type_ = type;
    // simple debug colors per type
    switch (type_) {
        case TileType::Solid:  shape.setFillColor(sf::Color::Green);  break;
        case TileType::OneWay: shape.setFillColor(sf::Color::Cyan);   break;
        case TileType::Death:  shape.setFillColor(sf::Color::Red);    break;
        case TileType::Void:   shape.setFillColor(sf::Color::Black);  break;
    }
}

sf::FloatRect StageObject::getBounds() const {
    return shape.getGlobalBounds();
}

void StageObject::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}