#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <string>
#include <Types.h>

class GameObject {
protected:
    OBJECT_ID_TYPE id;
    sf::RectangleShape shape;
    sf::Texture* texture = nullptr;

public:
    GameObject(OBJECT_ID_TYPE id, const sf::Vector2f& size, const sf::Vector2f& position)
        : id(id), shape(size)
    {
        shape.setPosition(position);
    }

    GameObject(){};

    virtual ~GameObject() = default;

    OBJECT_ID_TYPE getId() const { return id; }
    void setId(OBJECT_ID_TYPE newId) { id = newId; }

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
