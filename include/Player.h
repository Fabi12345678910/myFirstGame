#pragma once
#include "Collidable.h"
#include "Movable.h"
#include "GameObject.h"
#include <SFML/Graphics.hpp>

class Player : public Collidable, public Movable, public GameObject {
private:
    bool  isOnGround = false;
    float gravity    = 800.f;
    float speed      = 400.f;
    float health     = 10.f;
    int16_t  projectileCooldown   = 0; // bullet cooldown
    bool  facing     = FACING_LEFT; // -1 if looking left, 1 if looking right

public:
    static const bool FACING_LEFT = false;
    static const bool FACING_RIGHT = true;

    Player(OBJECT_ID_TYPE id, sf::Vector2f size, sf::Vector2f position)
        : GameObject(id, size, position) {}

    bool  getIsOnGround() const { return isOnGround; }
    void  setIsOnGround(bool v) { isOnGround = v; }

    float getGravity() const { return gravity; }
    void  setGravity(float g) { gravity = g; }

    float getSpeed() const { return speed; }
    void  setSpeed(float s) { speed = s; }

    float getHealth() const { return health; }
    void  setHealth(float h) { health = h; }

    int16_t getProjectileCooldown() const { return projectileCooldown; }
    void setProjectileCooldown(int16_t c) { projectileCooldown = c; }

    bool getFacing() const { return facing; }
    void setFacing(bool f) { facing = f; }
    // Player(float width, float height, float x, float y);
    // sf::FloatRect getBounds() const override;
    // void update(float dt) override;
    // void onCollision(StaticCollidable& other) override;
    // void draw(sf::RenderWindow &window) const override;
};