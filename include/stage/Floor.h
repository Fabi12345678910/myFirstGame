#pragma once
#include "../StaticCollidable.h"

class Floor : public StaticCollidable{
private:
    sf::RectangleShape shape;

public:
    Floor(float width, float height, float x, float y);
    sf::FloatRect getBounds() const override;
    void onCollision(Collidable& other) override;
    void draw(sf::RenderWindow& window) const override;
};