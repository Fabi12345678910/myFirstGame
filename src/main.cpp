#include "Game.h"
#include "GameState.h"
#include "Stage.h"

GameState createLocalGame(){

    std::vector<StageObject> stageObjects;
    auto so = new StageObject(0, sf::Vector2f(800.f, 50.f), sf::Vector2f(0.f,550.f));
    stageObjects.push_back(StageObject(0, sf::Vector2f(800.f, 50.f), sf::Vector2f(0.f,550.f)));
    stageObjects.push_back(StageObject(1, sf::Vector2f(300.f, 50.f), sf::Vector2f(0.f,450.f)));
    stageObjects.push_back(StageObject(2, sf::Vector2f(100.f, 50.f), sf::Vector2f(500.f,350.f)));
    stageObjects[0].getShape().setFillColor(sf::Color::Green);
    stageObjects[1].getShape().setFillColor(sf::Color::Green);
    stageObjects[2].getShape().setFillColor(sf::Color::Green);
    std::vector<sf::Vector2f> spawnPoints = {sf::Vector2f(400.f,10.f)};
    Stage s = Stage(stageObjects, spawnPoints);
    GameState gs;
    gs.setStage(s);
    gs.addPlayer(Player(100, sf::Vector2f(40.f, 40.f), sf::Vector2f(400.f, 10.f)));
    gs.getPlayer(100).getShape().setFillColor(sf::Color::Magenta);
    return gs;
}

int main() {
    GameState gs = createLocalGame();
    Game game(gs, 100);
    game.run();
    
    return 0;
}
