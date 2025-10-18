#pragma once
#include <SFML/Graphics.hpp>
#include "Collidable.h"

class DynamicCollidable : public Collidable {
protected:
    sf::Vector2f velocity;

public:
    virtual void update(float dt) = 0;
};