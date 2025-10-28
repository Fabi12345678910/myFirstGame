#pragma once
#include <SFML/Graphics.hpp>

// Tile categories for different behaviors
enum class TileType { 
    Solid, 
    OneWay, 
    Death, 
    Void 
};

class StaticCollidable {
protected:
    sf::RectangleShape shape;
    TileType type_ = TileType::Solid;

public:
    virtual sf::FloatRect getBounds() const = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual ~StaticCollidable() = default;
    TileType getType() const { return type_; }
};
