#pragma once
#include <SFML/Graphics.hpp>
#include "GameState.h"

class Renderer
{
private:
    sf::RenderWindow& window;
public:
    Renderer(sf::RenderWindow& win) : window(win) {}
    ~Renderer(){};
    void processDisplayEvents();
    void render(GameState& gamestate);
    void renderWaitingMessage();
    void renderWaitingMessage(int numDots = 3);
    void renderReadyMessage(bool isReady);
    void display() {
        window.display();
    }
};