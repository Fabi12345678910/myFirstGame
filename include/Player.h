#pragma once
#include <SFML/Graphics.hpp>
#include "DynamicCollidable.h"
#include "StaticCollidable.h"

class Player : public DynamicCollidable{
private:
    bool isOnGround = false;
    float gravity = 800.f;
    float speed = 400.f;

public:
    Player(float width, float height, float x, float y);
    sf::FloatRect getBounds() const override;
    void update(float dt) override;
    void onCollision(StaticCollidable& other) override;
    void draw(sf::RenderWindow &window) const override;
};