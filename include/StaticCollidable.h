#pragma once
#include "SFML/Graphics.hpp"

class StaticCollidable {
protected:
    sf::RectangleShape shape;
public:
    virtual sf::FloatRect getBounds() const = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual ~StaticCollidable() = default;
};
