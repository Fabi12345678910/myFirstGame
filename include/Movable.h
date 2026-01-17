#pragma once
#include <SFML/System/Vector2.hpp>

class Movable{
private:
    sf::Vector2f velocity;

public:
    sf::Vector2f getVelocity(){return velocity;};
    void setVelocity(sf::Vector2f velocity){
        this->velocity = velocity;
    }
};