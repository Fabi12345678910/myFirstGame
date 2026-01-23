#include "Renderer.h"
#include "StageObject.h"
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <cmath>
#include <algorithm>
#include <optional>
#include "CircularArray.h"
#include "FontManager.h"

void Renderer::renderWaitingMessage() {
    //0  -199 = 1 dot
    //200-399 = 2 dots
    //400-599 = 3 dots
    
    int numDots;
    sf::Time elapsedWaitTime = waitingDotClock.getElapsedTime();
    if(elapsedWaitTime >= sf::milliseconds(400)){
        numDots = 3;
        waitingDotClock.restart();
    }else if(elapsedWaitTime >= sf::milliseconds(200)){
        numDots = 2;
    }else{
        numDots = 1;
    }

    sf::Font& font = FontManager::getDefaultFont();

    const float W = 1920.f;
    const float H = 1080.f;

    float topMargin = 0.06f * H;
    float textScale = 0.03f * H;

    int dots = std::max(1, std::min(numDots, 3));
    std::string message = "Waiting for players" + std::string(dots, '.');

    sf::Text text(font, message, textScale);
    text.setFillColor(sf::Color::White);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(2.f);

    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(sf::Vector2f(
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y  + bounds.size.y / 2.f
    ));

    text.setPosition(sf::Vector2f(960.f, topMargin));
    window.draw(text);
}

    void Renderer::renderFrameTimeGraph(const CircularArray<HEALTH_FRAME_TIME_TYPE, 256>& frameTimes, HEALTH_FRAME_TIME_TYPE criticalMs, float startX, float startY){
    constexpr float barWidth = 2.f;
    constexpr float barSpacing = 1.f;
    constexpr float graphHeight = 100.f; // max height in pixels

    size_t minIdx = frameTimes.getMinIndex();
    size_t maxIdx = frameTimes.getSize();

    for (size_t i = minIdx; i < maxIdx; ++i) {
        float ms = frameTimes[i];
        float height = std::min(ms * 5.f, graphHeight); // scale for visibility
        height = std::max(height, 1.f); //at least 1 to have some visibility

        sf::RectangleShape bar({barWidth, height});
        bar.setPosition(sf::Vector2f(startX + (i - minIdx) * (barWidth + barSpacing),
                        startY + graphHeight - height));

        // Color gradient
        if (ms < criticalMs) bar.setFillColor(sf::Color::Green);
        else bar.setFillColor(sf::Color::Red);
        ;

        window.draw(bar);
    }

    // Draw critical line
    float critHeight = std::min(criticalMs * 5.f, graphHeight);
    sf::RectangleShape criticalLine({(maxIdx - minIdx) * (barWidth + barSpacing), 1.f});
    criticalLine.setPosition(sf::Vector2f(startX, startY + graphHeight - critHeight));
    criticalLine.setFillColor(sf::Color::Magenta);
    window.draw(criticalLine);
}

void Renderer::renderServerQueueHealth(std::uint8_t health){
    float barWidth = 20.f; // width of each tick bar
    float spacing = 5.f;   // space between bars
    float startX = 50.f;   // left margin
    float startY = 100.f;   // top margin
    float barHeight = 20.f; // max height of bars

    constexpr uint8_t maxBars = 5;

    sf::Vector2f origin = sf::Vector2f(40, 200);

    const std::size_t activeBars =
        std::min(health, maxBars);

    for (std::size_t i = 0; i < maxBars; ++i) {
        sf::RectangleShape bar({barWidth, barHeight});
        bar.setPosition(
            sf::Vector2f(
            origin.x + i * (barWidth + spacing),
            origin.y)
        );

        if (i < activeBars) {
            bar.setFillColor(sf::Color::Cyan);
        } else {
            bar.setFillColor(sf::Color(60, 60, 60));
        }

        window.draw(bar);
    }
}


sf::Color getHealthColor(TickHealth::HealthState state) {
    switch(state) {
        case TickHealth::GENERATED: return sf::Color::Green;
        case TickHealth::READY_TO_GENERATE: return sf::Color::Yellow;
        case TickHealth::MISSING_SERVER_UPDATE_INFOS: return sf::Color::Red;
        case TickHealth::UNAVAILABLE: return sf::Color(128, 128, 128); // gray
        default: return sf::Color::Black;
    }
}

void Renderer::renderGameStateHealth(HealthReport& report){
    float barWidth = 20.f; // width of each tick bar
    float spacing = 5.f;   // space between bars
    float startX = 50.f;   // left margin
    float startY = 50.f;   // top margin
    float barHeight = 20.f; // max height of bars

    for (size_t i = 0; i < report.tickHealths.size(); ++i) {
        const TickHealth& tick = report.tickHealths[i];
        sf::RectangleShape bar;
        bar.setSize(sf::Vector2f(barWidth, barHeight));
        bar.setFillColor(getHealthColor(tick.healthState));
        bar.setPosition(sf::Vector2f(startX + i * (barWidth + spacing), startY));
        window.draw(bar);
    }
}

void Renderer::renderReadyMessage(bool isReady) {
    sf::Font& font = FontManager::getDefaultFont();

    const float W = 1920.f;
    const float H = 1080.f;

    float topMargin = 0.06f * H;
    float textScale = 0.03f * H;

    std::string ready = isReady ? "unready" : "ready";
    std::string message = "Press R to go " + ready;

    sf::Text text(font, message, textScale);
    text.setFillColor(sf::Color::White);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(2.f);

    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(sf::Vector2f(
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y  + bounds.size.y / 2.f
    ));    

    text.setPosition(sf::Vector2f(960.f, topMargin));    
    window.draw(text);
}

void Renderer::renderMapSelection(TICK_TYPE timeLeft, int16_t& selectedId, bool& confirmed, std::vector<std::pair<int16_t, std::string>> maps) {
    sf::Font& font = FontManager::getDefaultFont();

    const float W = 1920.f;
    const float H = 1080.f;

    float topMargin = 0.06f * H;
    float lineSpacing = 0.05f * H;
    float textScale = 0.03f * H;

    // ─── INPUT ───────────────────────────────────────────────
    while (auto eventOpt = window.pollEvent()) {
        
        const sf::Event& event = *eventOpt;

        if (event.is<sf::Event::Closed>())
            window.close();

        auto key = event.getIf<sf::Event::KeyPressed>();
        if (key && !confirmed)
        {
            switch (key->code)
            {
                case sf::Keyboard::Key::Down:
                case sf::Keyboard::Key::Left:
                    selectedId = (selectedId + 1) % maps.size();
                    break;

                case sf::Keyboard::Key::Up:
                case sf::Keyboard::Key::Right:
                    selectedId = (selectedId - 1 + maps.size()) % maps.size();
                    break;

                case sf::Keyboard::Key::Enter:
                    confirmed = true;
                    break;

                default:
                    break;
            }
        }
    }

    // ─── HEADER ──────────────────────────────────────────────
    int secondsLeft = timeLeft/100;
    std::string header = "Select Map " + std::to_string(secondsLeft);
    sf::Text headerText(font, header, textScale);
    headerText.setPosition(sf::Vector2f(W / 2.f, topMargin));
    headerText.setOrigin(headerText.getLocalBounds().getCenter());
    window.draw(headerText);

    // ─── MENU ────────────────────────────────────────────────
    std::string label = "<< " + std::to_string(maps[selectedId].first) + " " + maps[selectedId].second + " >>";
    sf::Text entry(font, label, textScale);

    entry.setPosition(sf::Vector2f(W / 2.f, topMargin + 100.f));
    entry.setOrigin(entry.getLocalBounds().getCenter());
    if (!confirmed) {
        entry.setFillColor(sf::Color::Red);
    }
    else {
        entry.setFillColor(sf::Color::Green);
    }

    //TODO: add a screenshot of the current map

    window.draw(entry);
}

void Renderer::renderLoading() {
    const float W = 1920.f;
    const float H = 1080.f;
    sf::Vector2f center(W / 2.f, H / 2.f);
    float radius = 60.f;
    float dotRadius = 12.f;
    int numDots = 12;
    float twoPi = 6.2831853f;
    for (int i = 0; i < numDots; ++i) {
        float t = static_cast<float>(i) / numDots;
        float theta = waitingAngle + t * twoPi;
        float x = center.x + radius * std::cos(theta);
        float y = center.y + radius * std::sin(theta);
        sf::CircleShape dot(dotRadius);
        dot.setOrigin(sf::Vector2f(dotRadius, dotRadius));
        // Fade effect for spinner
        int alpha = static_cast<int>(180 + 75 * std::sin(theta - waitingAngle));
        dot.setFillColor(sf::Color(255, 255, 255, alpha));
        dot.setPosition(sf::Vector2f(x, y));
        window.draw(dot);
    }
    waitingAngle+=0.2f;
}

void Renderer::renderGameStart(TICK_TYPE timeLeft) {
    sf::Font& font = FontManager::getDefaultFont();

    const float W = 1920.f;
    const float H = 1080.f;

    float topMargin = 0.5f * H;
    float textScale = 0.2f * H;

    int secondsLeft = timeLeft/100;
    std::string text = std::to_string(secondsLeft);
    sf::Text headerText(font, text, textScale);
    headerText.setPosition(sf::Vector2f(W / 2.f, topMargin));
    headerText.setOrigin(headerText.getLocalBounds().getCenter());
    window.draw(headerText);
}

void Renderer::renderScore(const GameState& gameState, std::optional<OBJECT_ID_TYPE> winnerPlayerId) {
    sf::Font& font = FontManager::getDefaultFont();

    const float W = 1920.f;
    const float H = 1080.f;

    struct ScoreLine {
        OBJECT_ID_TYPE playerId;
        unsigned short score;
    };

    std::vector<ScoreLine> lines;
    lines.reserve(gameState.getPlayerCount());
    for (auto itPlayer = gameState.getPlayersBegin(); itPlayer != gameState.getPlayersEnd(); ++itPlayer) {
        const Player& p = itPlayer->second;
        lines.push_back({ p.getId(), p.getScore() });
    }

    std::sort(lines.begin(), lines.end(), [](const ScoreLine& a, const ScoreLine& b) {
        if (a.score != b.score) return a.score > b.score;
        return a.playerId < b.playerId;
    });

    const float titleSize = 0.06f * H;
    const float lineSize = 0.04f * H;
    const float panelWidth = 0.42f * W;
    const float panelPadding = 0.02f * W;
    const float lineGap = 0.012f * H;

    const float panelX = (W - panelWidth) / 2.f;
    const float panelY = 0.18f * H;

    const float panelHeight = panelPadding * 2.f
        + titleSize + lineGap
        + (winnerPlayerId.has_value() ? (lineSize + lineGap) : 0.f)
        + (lines.empty() ? lineSize : (lines.size() * (lineSize + lineGap)));

    sf::RectangleShape panel({ panelWidth, panelHeight });
    panel.setPosition({ panelX, panelY });
    panel.setFillColor(sf::Color(0, 0, 0, 140));
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color(255, 255, 255, 60));
    window.draw(panel);

    auto makeText = [&](const std::string& str, float size) {
        sf::Text t(font, str, static_cast<unsigned int>(size));
        t.setFillColor(sf::Color::White);
        t.setOutlineColor(sf::Color::Black);
        t.setOutlineThickness(2.f);
        return t;
    };

    float cursorY = panelY + panelPadding;

    {
        sf::Text title = makeText("Score", titleSize);
        title.setPosition({ panelX + panelWidth / 2.f, cursorY });
        title.setOrigin(title.getLocalBounds().getCenter());
        window.draw(title);
        cursorY += titleSize + lineGap;
    }

    if (winnerPlayerId.has_value()) {
        std::string winnerLine = (winnerPlayerId.value() >= 0)
            ? ("Winner: P" + std::to_string(winnerPlayerId.value()))
            : std::string("Winner: -");

        sf::Text winnerText = makeText(winnerLine, lineSize);
        winnerText.setPosition({ panelX + panelWidth / 2.f, cursorY });
        winnerText.setOrigin(winnerText.getLocalBounds().getCenter());
        winnerText.setFillColor(sf::Color(255, 220, 60));
        window.draw(winnerText);
        cursorY += lineSize + lineGap;
    }

    if (lines.empty()) {
        sf::Text emptyText = makeText("No players", lineSize);
        emptyText.setPosition({ panelX + panelWidth / 2.f, cursorY });
        emptyText.setOrigin(emptyText.getLocalBounds().getCenter());
        window.draw(emptyText);
        return;
    }

    // Compute a stable name column width for alignment.
    float maxNameWidth = 0.f;
    for (const auto& l : lines) {
        sf::Text tmp = makeText("P" + std::to_string(l.playerId), lineSize);
        maxNameWidth = std::max(maxNameWidth, tmp.getLocalBounds().size.x);
    }

    const float leftX = panelX + panelPadding;
    const float rightX = panelX + panelWidth - panelPadding;
    const float nameX = leftX;
    const float scoreX = std::max(nameX + maxNameWidth + 40.f, rightX - 50.f);

    for (const auto& l : lines) {
        sf::Text name = makeText("P" + std::to_string(l.playerId), lineSize);
        name.setPosition({ nameX, cursorY });

        sf::Text score = makeText(std::to_string(l.score), lineSize);
        score.setPosition({ scoreX, cursorY });
        score.setOrigin({ score.getLocalBounds().size.x, 0.f });

        if (winnerPlayerId.has_value() && l.playerId == winnerPlayerId.value()) {
            name.setFillColor(sf::Color(255, 220, 60));
            score.setFillColor(sf::Color(255, 220, 60));
        }

        window.draw(name);
        window.draw(score);

        cursorY += lineSize + lineGap;
    }
}

void Renderer::renderWinner(std::optional<OBJECT_ID_TYPE> winnerPlayerId) {
    sf::Font& font = FontManager::getDefaultFont();

    const float W = 1920.f;
    const float H = 1080.f;

    const float titleSize = 0.075f * H;

    const float t = winnerBlinkClock.getElapsedTime().asSeconds();
    const bool visible = std::fmod(t, 1.0f) < 0.55f;
    if (!visible) {
        return;
    }

    std::string line;
    if (winnerPlayerId.has_value() && winnerPlayerId.value() >= 0) {
        line = "Player " + std::to_string(winnerPlayerId.value()) + " won";
    } else {
        line = "Winner unknown";
    }

    sf::Text text(font, line, static_cast<unsigned int>(titleSize));
    text.setFillColor(sf::Color(255, 220, 60));
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(2.f);
    text.setPosition({ W / 2.f, H / 2.f });
    text.setOrigin(text.getLocalBounds().getCenter());
    window.draw(text);
}

// simple color selector for tile types
static sf::Color colorForStageType(StageObjectType t) {
    switch (t) {
        case StageObjectType::Solid:      return sf::Color(130, 130, 130);   // gray
        case StageObjectType::HalfSolid:  return sf::Color(90, 170, 255);    // blue
        case StageObjectType::Death: return sf::Color(220, 60, 30);     // red/orange
        default:                          return sf::Color::White;
    }
}

void Renderer::render(GameState& gameState) {
    window.clear(sf::Color(25, 25, 28)); // dark background

    // ---- Stage ----
    for (const StageObject& obj : gameState.getStage().getStageObjects()) {
        sf::RectangleShape shape = obj.getShape();
        shape.setFillColor(colorForStageType(obj.getType()));
        shape.setOutlineThickness(1.f);
        shape.setOutlineColor(sf::Color(0, 0, 0, 50));
        window.draw(shape);
    }

    // ---- Spawn points (optional visualization) ----
    for (const sf::Vector2f& sp : gameState.getStage().getSpawnPoints()) {
        sf::CircleShape dot{5.f};
        dot.setOrigin({5.f, 5.f});
        dot.setPosition(sp);
        dot.setFillColor(sf::Color{60, 230, 90});
        window.draw(dot);
    }

    // ---- Players ----
    for (auto itPlayer = gameState.getPlayersBegin(); itPlayer != gameState.getPlayersEnd(); itPlayer++){
        if (itPlayer->second.getHealth() <= 0) {continue;}
        sf::RectangleShape rect = itPlayer->second.getShape();
        rect.setFillColor(itPlayer->second.getShape().getFillColor());
        rect.setOutlineThickness(1.f);
        rect.setOutlineColor(sf::Color(0, 0, 0, 70));
        window.draw(rect);
    }

    // ---- Projectiles ----
    for (auto itProjectile = gameState.getProjectilesBegin(); itProjectile != gameState.getProjectilesEnd(); itProjectile++){
        sf::RectangleShape rect = itProjectile->second.getShape();
        rect.setFillColor(sf::Color(255, 220, 60));
        rect.setOutlineThickness(1.f);
        rect.setOutlineColor(sf::Color(0, 0, 0, 70));
        window.draw(rect);
    }

    //window.display();
}

void Renderer::processDisplayEvents() {
    while (const std::optional<sf::Event> maybeEvent = window.pollEvent()) {
        const sf::Event& event = *maybeEvent;
        if (event.is<sf::Event::Closed>()) {
            window.close();
            exit(EXIT_SUCCESS);
        }
    }
}
