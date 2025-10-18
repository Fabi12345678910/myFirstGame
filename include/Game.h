#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "stage/Floor.h"

class Game {
public:
    Game();
    void run();
private:
    void processEvents();
    void update(float deltaTime);
    void render();

    sf::RenderWindow window;
    Player player;
    Floor floor;
};
