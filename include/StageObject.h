#pragma once
#include <SFML/Graphics.hpp>
#include <Collidable.h>

class StageObject : public Collidable, public GameObject {
public:
    StageObject(OBJECT_ID_TYPE id, sf::Vector2f size, sf::Vector2f position) : GameObject(id, size, position) {}
};          