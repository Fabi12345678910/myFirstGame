#include "Renderer.h"
#include "StageObject.h"

// simple color selector for tile types
static sf::Color colorForStageType(StageObjectType t) {
    switch (t) {
        case StageObjectType::Solid:      return sf::Color(130, 130, 130);   // gray
        case StageObjectType::SemiSolid:  return sf::Color(90, 170, 255);    // blue
        case StageObjectType::HazardLava: return sf::Color(220, 60, 30);     // red/orange
        case StageObjectType::HazardSpike:return sf::Color(200, 200, 220);   // light gray
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
        sf::RectangleShape rect = player.getShape();
        rect.setFillColor(sf::Color(255, 220, 60));
        rect.setOutlineThickness(1.f);
        rect.setOutlineColor(sf::Color(0, 0, 0, 70));
        window.draw(rect);
    }

    window.display();
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
