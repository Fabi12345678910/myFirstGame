#pragma once
#include <SFML/Graphics.hpp>
#include "GameState.h"

class Renderer
{
private:
    /* data */
    sf::RenderWindow window;
public:
    Renderer(): window(sf::VideoMode(sf::Vector2u(800, 600)), "My Game"){};
    ~Renderer(){};
    void processDisplayEvents();
    void render(GameState& gamestate);
};