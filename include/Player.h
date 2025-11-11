#pragma once
#include <collision/Collidable.h>
#include "Movable.h"
#include "GameObject.h"
#include <SFML/Graphics.hpp>

class Player : public Collidable, public Movable, public GameObject {
private:
    bool  isOnGround = false;
    bool  isEliminated = false; // removed for current round
    float gravity    = 800.f;
    float speed      = 400.f;
    float health     = 10.f;

public:
    Player(OBJECT_ID_TYPE id, sf::Vector2f size, sf::Vector2f position)
        : GameObject(id, size, position) {}

    bool  getIsOnGround() const { return isOnGround; }
    void  setIsOnGround(bool v) { isOnGround = v; }

    bool  getIsEliminated() const { return isEliminated; }
    void  setIsEliminated(bool v) { isEliminated = v; }

    float getGravity() const { return gravity; }
    void  setGravity(float g) { gravity = g; }

    float getSpeed() const { return speed; }
    void  setSpeed(float s) { speed = s; }

    float getHealth() const { return health; }
    void  setHealth(float h) { health = h; }

    virtual void accept(CollidableVisitor& v) override{
        v.visit(*this);
    }
    
    // Player(float width, float height, float x, float y);
    // sf::FloatRect getBounds() const override;
    // void update(float dt) override;
    // void onCollision(StaticCollidable& other) override;
    // void draw(sf::RenderWindow &window) const override;
};