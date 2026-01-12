#pragma once
#include <SFML/Graphics.hpp>
#include "GameState.h"
#include "GameStateHealth.h"

class Renderer
{
private:
    sf::RenderWindow& window;
public:
    Renderer(sf::RenderWindow& win) : window(win) {}
    ~Renderer(){};
    void processDisplayEvents();
    void render(GameState& gamestate);
    void renderWaitingMessage();
    void renderGameStateHealth(HealthReport& report);
    void renderWaitingMessage(int numDots = 3);
    void renderReadyMessage(bool isReady);
    void renderMapSelection(TICK_TYPE timeLeft, int16_t& selectionIndex, bool& confirmed, std::vector<std::pair<int16_t, std::string>> maps);
    void renderLoading(float angle);
    void renderGameStart(TICK_TYPE gameStartTick);
    sf::RenderWindow& getWindow() {
        return window;
    }
    void renderServerQueueHealth(std::uint8_t health);
    void display() {
        window.display();
    }
};