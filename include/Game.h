#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "StaticCollidable.h"

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
    std::vector<StaticCollidable*> staticCollidables;

    sf::FloatRect voidZone;
};

