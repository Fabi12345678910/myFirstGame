#pragma once
#include "SFML/Graphics.hpp"
#include "StaticCollidable.h"

class DynamicCollidable {
protected:
    sf::Vector2f velocity;
    sf::RectangleShape shape;

public:
    virtual void update(float dt) = 0;
    virtual sf::FloatRect getBounds() const = 0;
    virtual void onCollision(StaticCollidable& other) = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual ~DynamicCollidable() = default;
};