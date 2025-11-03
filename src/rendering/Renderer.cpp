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