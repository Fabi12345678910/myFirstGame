#pragma once
#include "Collidable.h"
#include "Movable.h"
#include "GameObject.h"
#include <SFML/Graphics.hpp>

class Projectile : public Collidable, public Movable, public GameObject {
private:
    bool  isActive = true;
    float speed    = 800.f;

public:
    Projectile(OBJECT_ID_TYPE id, sf::Vector2f size, sf::Vector2f position)
        : GameObject(id, size, position) {}

    float getSpeed() const { return speed; }
    void  setSpeed(float s) { speed = s; }

    float getIsActive() const { return isActive; }
    void  setIsActive(bool a) { isActive = a; }
};