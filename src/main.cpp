#include "Game.h"
#include "GameState.h"
#include "maps/Map_TestAll.h"

GameState createGameFromStage(const Stage& stage) {
    GameState gs;
    gs.setStage(const_cast<Stage&>(stage)); // Stage copied; const_cast OK for now
    gs.addPlayer(Player(100, sf::Vector2f{40.f, 40.f}, sf::Vector2f{400.f, 10.f}));
    gs.getPlayer(100).getShape().setFillColor(sf::Color::Magenta);
    return gs;
}

int main() {
    Stage stage = createMap_TestAll();
    GameState gs = createGameFromStage(stage);
    Game game(gs, 100);
    game.run();
    return 0;
}
