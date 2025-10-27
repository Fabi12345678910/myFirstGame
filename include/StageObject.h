#pragma once
#include <SFML/Graphics.hpp>
#include <Collidable.h>

class StageObject : public Collidable {
public:
    StageObject(sf::Vector2f size, sf::Vector2f position) {
        this->shape.setSize(size);
        this->shape.setPosition(position);
    }
};          