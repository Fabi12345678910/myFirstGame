#include "Game.h"
#include "StageObject.h"
#include <iostream>

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

void onCollision(Player& player, GameObject const & other) {
    sf::FloatRect playerBounds = player.getShape().getGlobalBounds();
    sf::FloatRect otherBounds = other.getShape().getGlobalBounds();
    sf::Vector2f playerVelocity = player.getVelocity();

    float dx = (playerBounds.position.x + playerBounds.size.x / 2.f) 
         - (otherBounds.position.x + otherBounds.size.x / 2.f);

    float dy = (playerBounds.position.y + playerBounds.size.y / 2.f) 
         - (otherBounds.position.y + otherBounds.size.y / 2.f);


    float combinedHalfWidths = (playerBounds.size.x / 2.f) + (otherBounds.size.x / 2.f);
    float combinedHalfHeights = (playerBounds.size.y / 2.f) + (otherBounds.size.y / 2.f);

    // Only handle if actually colliding
    float overlapX = combinedHalfWidths - abs(dx);
    float overlapY = combinedHalfHeights - abs(dy);

    if (overlapX < overlapY) {
        // Horizontal collision
        if (dx > 0.f) {
            // Player is on the right
            player.getShape().move(sf::Vector2f(overlapX, 0.f));
            std::cout << "Collision from left\n";
        } else {
            // Player is on the left
            player.getShape().move(sf::Vector2f(-overlapX, 0.f));
            std::cout << "Collision from right\n";
        }
        playerVelocity.x = 0.f;
        player.setVelocity(playerVelocity);
    } else {
        // Vertical collision
        if (dy > 0.f) {
            // Player is below
            player.getShape().move(sf::Vector2f(0.f, overlapY));
            playerVelocity.y = 0.f;
            player.setVelocity(playerVelocity);
            std::cout << "Collision from above\n";
        } else {
            // Player is above
            player.getShape().move(sf::Vector2f(0.f, -overlapY));
            playerVelocity.y = 0.f;
            player.setVelocity(playerVelocity);
            player.setIsOnGround(true);
            std::cout << "Collision from below / landed\n";
        }
    }
}

void Game::update(float deltaTime) {
    for(Player& player : gameState.getPlayers()){

    player.setIsOnGround(false);

    sf::Vector2f playerVelocity = player.getVelocity();
    if (!player.getIsOnGround()) {
        playerVelocity.y += player.getGravity() * deltaTime;
    } else {
        playerVelocity.y = 0.f;
    }
    player.setVelocity(playerVelocity);

    player.getShape().move(player.getVelocity() * deltaTime);

    //somehow check all objects, idk how yet
    for (StageObject const &stageObject: gameState.getStage().getStageObjects()){

        const Collidable *collidable = dynamic_cast<const Collidable*>(&stageObject);
        if(collidable != NULL){

        if (player.getShape().getGlobalBounds().findIntersection(stageObject.getShape().getGlobalBounds())) {
            std::cout << "detected collision\n";
            onCollision(player, stageObject);
        }}
    }
    }
}


void Game::render() {
    window.clear(sf::Color::Yellow);
    for(Player& player:gameState.getPlayers()){
        window.draw(player.getShape());
    }
    for (StageObject const& stageObject : gameState.getStage().getStageObjects()) {
        window.draw(stageObject.getShape());
    }

    window.display();
}
