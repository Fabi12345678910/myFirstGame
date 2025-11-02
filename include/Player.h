#pragma once
#include <SFML/Graphics.hpp>
#include <Collidable.h>
#include <Movable.h>

class Player : public Collidable, public Movable, public GameObject{
private:
    bool isOnGround = false;
    float gravity = 800.f;
    float speed = 400.f;
    float health = 10;

public:
    Player(int id, sf::Vector2f size, sf::Vector2f position) : GameObject(id, size, position){}


    float getIsOnGround() const { return isOnGround; }
    void setIsOnGround(bool isOnGround) { this->isOnGround = isOnGround; }

    float getGravity() const { return gravity; }
    void setGravity(float gravity) { this->gravity = gravity; }

    float getSpeed() const { return speed; }
    void setSpeed(float gravity) { this->speed = speed; }

    float getHealth() const { return health; }
    void setHealth(float health) { this->health = health; }

    // Player(float width, float height, float x, float y);
    // sf::FloatRect getBounds() const override;
    // void update(float dt) override;
    // void onCollision(StaticCollidable& other) override;
    // void draw(sf::RenderWindow &window) const override;
};