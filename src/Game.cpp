#include "Game.h"
#include "StageObject.h"
#include <iostream>

Game::Game()
: window(sf::VideoMode(sf::Vector2u(800, 600)), "My Game"),
  player(40.f, 40.f, 400.f, 10.f)
{
    staticCollidables.push_back(new StageObject(350.f, 50.f, 0.f, 550.f));   // left floor piece
    staticCollidables.push_back(new StageObject(350.f, 50.f, 450.f, 550.f)); // right floor piece
    staticCollidables.push_back(new StageObject(300.f, 50.f, 0.f, 450.f));  // left middle platform
    staticCollidables.push_back(new StageObject(100.f, 50.f, 500.f, 350.f));    // right high platform
    staticCollidables.push_back(new StageObject(150.f, 20.f, 325.f, 300.f, TileType::OneWay));  // one-way platform

    voidZone = sf::FloatRect(
        sf::Vector2f(350.f, 550.f),   // position (x, y)
        sf::Vector2f(100.f, 50.f)     // size (width, height)
);

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
    // sf::Event event;
    // while (window.pollEvent(event)) {
    //     if (event.type == sf::Event::Closed)
    //         window.close();
    // }
}

void Game::update(float deltaTime) {
    player.update(deltaTime);
    for (StaticCollidable* c : staticCollidables) {
        if (player.getBounds().findIntersection(c->getBounds())) {
            // std::cout << "detected collision\n";
            player.onCollision(*c);
        }
    }

    auto rectsOverlap = [](const sf::FloatRect& a, const sf::FloatRect& b) {
        return (a.position.x < b.position.x + b.size.x) &&
               (a.position.x + a.size.x > b.position.x) &&
               (a.position.y < b.position.y + b.size.y) &&
               (a.position.y + a.size.y > b.position.y);
    };

    sf::FloatRect playerBounds = player.getBounds();
    if (rectsOverlap(playerBounds, voidZone)) {
        // same X lane, appear just above the screen; keep velocity as-is
        sf::Vector2f pos = player.getPosition();
        float newY = -playerBounds.size.y; // spawn just above top
        player.setPosition({ pos.x, newY });
        // (no velocity change; player keeps falling naturally)
        std::cout << "VOID wrap: player reappears at top!\n";
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
