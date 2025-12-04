#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "GameState.h"

class Game {
public:
    Game(){
        window = sf::RenderWindow(sf::VideoMode(sf::Vector2u(800, 600)), "My Game");
    }
    Game(GameState gameState, int activePlayer){
        window = sf::RenderWindow(sf::VideoMode(sf::Vector2u(800, 600)), "My Game");
        this->gameState = gameState;
        this->activePlayer = activePlayer;
    }
    void run();
private:
    void processEvents();
    void update(float deltaTime);
    void render();
    void processInputs();
    GameState gameState;
    sf::RenderWindow window;
    int activePlayer;
};
