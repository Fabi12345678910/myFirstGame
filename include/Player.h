#pragma once
#include <SFML/Graphics.hpp>
#include "DynamicCollidable.h"
#include "StaticCollidable.h"

class Player : public DynamicCollidable{
private:
    bool isOnGround = false;
    float gravity = 800.f;
    float speed = 400.f;

    sf::Vector2f prevPosition;

public:
    Player(float width, float height, float x, float y);

    sf::FloatRect getBounds() const override;
    void update(float dt) override;
    void onCollision(StaticCollidable& other) override;
    void draw(sf::RenderWindow &window) const override;

    void setPosition(const sf::Vector2f& p);
    sf::Vector2f getPosition() const;
    void respawn (const sf::Vector2f& spawn);
};