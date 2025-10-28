#pragma once
#include <SFML/Graphics.hpp>
#include <StaticCollidable.h>

class StageObject : public StaticCollidable {
public:
    StageObject(float width, float height, float x, float y, TileType type = TileType::Solid);
    sf::FloatRect getBounds() const override;
    void draw(sf::RenderWindow &window) const override;
};