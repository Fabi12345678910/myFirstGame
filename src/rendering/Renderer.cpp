#include "Renderer.h"

void Renderer::render(GameState& gameState){
    window.clear(sf::Color::Yellow);
    for(Player& player:gameState.getPlayers()){
        window.draw(player.getShape());
    }
    for (StageObject const& stageObject : gameState.getStage().getStageObjects()) {
        window.draw(stageObject.getShape());
    }

    window.display();
    
}

void Renderer::processDisplayEvents() {
    while (const std::optional<sf::Event> maybeEvent = window.pollEvent()) {
        const sf::Event &event = *maybeEvent;
        if (event.is<sf::Event::Closed>())
            {
                window.close();
                exit(EXIT_SUCCESS);
            }
    }
}