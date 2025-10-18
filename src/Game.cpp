#include "Game.h"

Game::Game():
window(sf::VideoMode(800, 600), "My Game"), 
player(40.f, 40.f, 400.f, 200.f), 
floor(800.f, 50.f, 0.f, 550.f) 
{

}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void Game::update(float deltaTime) {
    if (player.getBounds().intersects(floor.getBounds())) {
        player.onCollision(floor);
    }
    player.update(deltaTime);
}

void Game::render() {
    window.clear(sf::Color::Yellow);
    player.draw(window);
    floor.draw(window);
    window.display();
}
