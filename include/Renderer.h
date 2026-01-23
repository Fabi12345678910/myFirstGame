#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include "GameState.h"
#include "GameStateHealth.h"
#include "Networking/EventDefinitions/EventServerHealth.h"
#include "CircularArray.h"
#include "Types.h"

class Renderer
{
private:
    sf::RenderWindow& window;
    sf::Clock waitingDotClock;
    sf::Clock winnerBlinkClock;
    float waitingAngle = 0;
public:
    Renderer(sf::RenderWindow& win) : window(win) {}
    ~Renderer(){waitingDotClock.start();};
    void renderFrameTimeGraph(const CircularArray<HEALTH_FRAME_TIME_TYPE, 256>& frameTimes, HEALTH_FRAME_TIME_TYPE criticalMs = 33, float startX = 50.f, float startY = 100.f);
    void processDisplayEvents();
    void render(GameState& gamestate);
    void renderWaitingMessage();
    void renderGameStateHealth(HealthReport& report);
    void renderReadyMessage(bool isReady);
    void renderMapSelection(TICK_TYPE timeLeft, int16_t& selectionIndex, bool& confirmed, std::vector<std::pair<int16_t, std::string>> maps);
    void renderLoading();
    void renderGameStart(TICK_TYPE gameStartTick);
    void renderScore(const GameState& gameState, std::optional<OBJECT_ID_TYPE> winnerPlayerId = std::nullopt);
    void renderWinner(std::optional<OBJECT_ID_TYPE> winnerPlayerId);
    sf::RenderWindow& getWindow() {
        return window;
    }
    void renderServerQueueHealth(std::uint8_t health);
    void display() {
        window.display();
    }
};