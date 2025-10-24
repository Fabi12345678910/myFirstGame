#include "Game.h"
#include "StageObject.h"

Game::Game()
: window(sf::VideoMode(800, 600), "My Game"),
  player(40.f, 40.f, 400.f, 510.f)
{
    staticCollidables.push_back(new StageObject(800.f, 50.f, 0.f, 550.f));
    staticCollidables.push_back(new StageObject(300.f, 50.f, 0.f, 450.f));
    staticCollidables.push_back(new StageObject(100.f, 50.f, 500.f, 350.f));
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
    player.update(deltaTime);
    for (StaticCollidable* c : staticCollidables) {
        if (player.getBounds().intersects(c->getBounds())) {
            player.onCollision(*c);
        }
    }
}

void Game::render() {
    window.clear(sf::Color::Yellow);
    player.draw(window);

    for (StaticCollidable* c : staticCollidables) {
        c->draw(window);
    }

    window.display();
}
