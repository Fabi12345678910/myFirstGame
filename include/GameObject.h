#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class GameObject {
protected:
    int id;
    sf::RectangleShape shape;
    sf::Texture* texture = nullptr;

public:
    GameObject(int id, const sf::Vector2f& size, const sf::Vector2f& position)
        : id(id)
    {
        shape.setSize(size);
        shape.setPosition(position);
    }

    virtual ~GameObject() = default;

    int getId() const { return id; }
    void setId(int newId) { id = newId; }

    const sf::RectangleShape& getShape() const { return shape; }
    sf::RectangleShape& getShape() { return shape; } // non-const for modification

    sf::Vector2f getPosition() const { return shape.getPosition(); }
    void setPosition(const sf::Vector2f& pos) { shape.setPosition(pos); }

    sf::Vector2f getSize() const { return shape.getSize(); }
    void setSize(const sf::Vector2f& size) { shape.setSize(size); }

    void setTexture(sf::Texture* tex) { 
        texture = tex; 
        shape.setTexture(texture);
    }
    sf::Texture* getTexture() const { return texture; }
};
