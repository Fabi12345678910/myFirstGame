#pragma once
#include <SFML/Graphics.hpp>

class Collidable {
public:
    virtual sf::FloatRect getBounds() const = 0;
    virtual void onCollision(Collidable& other) = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual ~Collidable() = default;
};
