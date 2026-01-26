#pragma once
#include "Collidable.h"
#include "Config.h"
#include "Movable.h"
#include "GameObject.h"
#include <SFML/Graphics.hpp>

class Projectile : public Collidable, public Movable, public GameObject {
private:
    float speed    = defaultProjectileSpeed;

public:
    Projectile(OBJECT_ID_TYPE id, sf::Vector2f size, sf::Vector2f position)
        : GameObject(id, size, position) {}

    float getSpeed() const { return speed; }
    void  setSpeed(float s) { speed = s; }
};