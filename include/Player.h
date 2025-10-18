#pragma once
#include <SFML/Graphics.hpp>
#include "DynamicCollidable.h"
#include "stage/Floor.h"

class Player : public DynamicCollidable{
private:
    sf::RectangleShape shape;
    bool isOnGround = false;
    float gravity = 800.f;
    float speed = 400.f;

public:
    Player(float width, float height, float x, float y);
    sf::FloatRect getBounds() const override;
    void update(float dt) override;
    void onCollision(Collidable& other) override;
    void onCollision(Floor& floor);
    void draw(sf::RenderWindow &window) const override;
};