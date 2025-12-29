#include "Renderer.h"
#include "StageObject.h"
#include <cctype>
#include <sstream>

void Renderer::renderWaitingMessage(int numDots) {
    sf::Font font;
    if(!font.openFromFile("../assets/fonts/PressStart2P-Regular.ttf")) {
        //std::cout << "failed loading font\n";
    }

    float w = window.getSize().x;
    float h = window.getSize().y;
    float topMargin = 0.06f * h; // start menu 20% from top
    float textScale = 0.03f;

        // Clamp numDots between 1 and 3
        int dots = std::max(1, std::min(numDots, 3));
        std::string dotsStr(dots, '.');
        std::string message = "Waiting for players" + dotsStr;

        sf::Text text(font, message, h*textScale);
    text.setFillColor(sf::Color::White);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(2.f);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(sf::Vector2f(textRect.position.x + textRect.size.x / 2.0f, textRect.size.y + textRect.position.y / 2.0f));
    text.setPosition(sf::Vector2f(w / 2.0f, topMargin));
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
