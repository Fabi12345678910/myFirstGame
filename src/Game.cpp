#include "Game.h"
#include "StageObject.h"
#include <iostream>
#include "GameUpdate.h"

static sf::Color colorFor(StageObjectType t) {
    switch (t) {
        case StageObjectType::Solid:         return sf::Color{130,130,130};  // gray
        case StageObjectType::SemiSolid:     return sf::Color{90,170,255};   // blue
        case StageObjectType::Hazard:         return sf::Color{220,60,30};    // red
        case StageObjectType::JumpPad:       return sf::Color{250,220,60};   // yellow
        case StageObjectType::MovingPlatform:return sf::Color{150,110,200};  // purple
        default:                             return sf::Color{180,180,180};
    }
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        processEvents();
        processInputs();
        update(deltaTime);
        render();
    }
}

void Game::processInputs(){
    sf::Vector2f playerVelocity = gameState.getPlayer(activePlayer).getVelocity();
    playerVelocity.x = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        playerVelocity.x -= gameState.getPlayer(activePlayer).getSpeed();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        playerVelocity.x += gameState.getPlayer(activePlayer).getSpeed();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && gameState.getPlayer(activePlayer).getIsOnGround()) {
        playerVelocity.y = -400.f;
        gameState.getPlayer(activePlayer).setIsOnGround(false);
    }
    gameState.getPlayer(activePlayer).setVelocity(playerVelocity);
}

void Game::processEvents() {
    while (const std::optional<sf::Event> maybeEvent = window.pollEvent()) {
        const sf::Event &event = *maybeEvent;
        if (event.is<sf::Event::Closed>())
            {
                window.close();
            }
    }
}

void Game::update(float deltaTime) {
    updateGame(gameState, deltaTime);
}

void Game::render() {
    // 1) Background
    window.clear(sf::Color{25,25,28}); // dark gray

    // 2) Ground (tiles) — visible rectangles with per-type color
    for (const StageObject& obj : gameState.getStage().getStageObjects()) {
        sf::RectangleShape rect = obj.getShape();
        rect.setFillColor(colorFor(obj.getType()));         // <<< important
        rect.setOutlineThickness(1.f);
        rect.setOutlineColor(sf::Color{0,0,0,50});
        window.draw(rect);
    }

    // 3) Players
    for (const Player& p : gameState.getPlayers()) {
        sf::RectangleShape rect = p.getShape();
        rect.setOutlineThickness(1.f);
        rect.setOutlineColor(sf::Color{20,20,20,90});
        if (!p.getTexture()) rect.setFillColor(sf::Color{255,220,60});
        window.draw(rect);
    }

    // 4) Foreground overlays — none yet

    // spawn points
    for (const sf::Vector2f& sp : gameState.getStage().getSpawnPoints()) {
        sf::CircleShape dot{5.f};
        dot.setOrigin(sf::Vector2f{5.f,5.f});
        dot.setPosition(sp);
        dot.setFillColor(sf::Color{60,230,90});
        window.draw(dot);
    }

    window.display();
}