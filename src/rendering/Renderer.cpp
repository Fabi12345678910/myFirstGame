#include "Renderer.h"
#include "StageObject.h"
#include "StageManager.h"
#include <cctype>
#include <sstream>
#include "plog/Log.h"
#include <cmath>

void Renderer::renderWaitingMessage(int numDots) {
    sf::Font font;
    if(!font.openFromFile("../assets/fonts/PressStart2P-Regular.ttf")) {
        PLOG_ERROR << "failed loading font\n";
    }

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

void Renderer::renderReadyMessage(bool isReady) {
    sf::Font font;
    if(!font.openFromFile("../assets/fonts/PressStart2P-Regular.ttf")) {
        PLOG_ERROR << "failed loading font\n";
    }

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
    sf::Font font;
    if(!font.openFromFile("../assets/fonts/PressStart2P-Regular.ttf")) {
        PLOG_ERROR << "failed loading font\n";
    }

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

void Renderer::renderLoading(float angle) {
    const float W = 1920.f;
    const float H = 1080.f;
    sf::Vector2f center(W / 2.f, H / 2.f);
    float radius = 60.f;
    float dotRadius = 12.f;
    int numDots = 12;
    float twoPi = 6.2831853f;
    for (int i = 0; i < numDots; ++i) {
        float t = static_cast<float>(i) / numDots;
        float theta = angle + t * twoPi;
        float x = center.x + radius * std::cos(theta);
        float y = center.y + radius * std::sin(theta);
        sf::CircleShape dot(dotRadius);
        dot.setOrigin(sf::Vector2f(dotRadius, dotRadius));
        // Fade effect for spinner
        int alpha = static_cast<int>(180 + 75 * std::sin(theta - angle));
        dot.setFillColor(sf::Color(255, 255, 255, alpha));
        dot.setPosition(sf::Vector2f(x, y));
        window.draw(dot);
    }
}

void Renderer::renderGameStart(TICK_TYPE timeLeft) {
    sf::Font font;
    if(!font.openFromFile("../assets/fonts/PressStart2P-Regular.ttf")) {
        PLOG_ERROR << "failed loading font\n";
    }

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
    for (Player& player : gameState.getPlayers()) {
        if (player.getHealth() <= 0) {continue;}
        sf::RectangleShape rect = player.getShape();
        rect.setFillColor(player.getShape().getFillColor());
        rect.setOutlineThickness(1.f);
        rect.setOutlineColor(sf::Color(0, 0, 0, 70));
        window.draw(rect);
    }

    // ---- Projectiles ----
    for (Projectile& projectile : gameState.getProjectiles()) {
        if (projectile.getIsActive()) {
            sf::RectangleShape rect = projectile.getShape();
            rect.setFillColor(sf::Color(255, 220, 60));
            rect.setOutlineThickness(1.f);
            rect.setOutlineColor(sf::Color(0, 0, 0, 70));
            window.draw(rect);
        }
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
