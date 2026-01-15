#include "maps/Map_TestAll.h"

Stage createMap_TestAll() {
    std::vector<StageObject> tiles;

    // === Bottom ground with gap ===
    tiles.emplace_back(0, sf::Vector2f{260.f, 50.f}, sf::Vector2f{0.f, 550.f}, StageObjectType::Solid);
    tiles.emplace_back(1, sf::Vector2f{140.f, 50.f}, sf::Vector2f{660.f, 550.f}, StageObjectType::Solid);

    // === Mid platform (solid) ===
    tiles.emplace_back(2, sf::Vector2f{400.f, 25.f}, sf::Vector2f{200.f, 400.f}, StageObjectType::Solid);

    // === Half-solid (blue) platforms above ===
    tiles.emplace_back(3, sf::Vector2f{160.f, 20.f}, sf::Vector2f{120.f, 320.f}, StageObjectType::HalfSolid);
    tiles.emplace_back(4, sf::Vector2f{160.f, 20.f}, sf::Vector2f{520.f, 260.f}, StageObjectType::HalfSolid);

    // === Jump Pad (yellow) on left ground ===
    tiles.emplace_back(5, sf::Vector2f{80.f, 15.f}, sf::Vector2f{90.f, 535.f}, StageObjectType::JumpPad);

    // === Death tile (red) near right ground gap ===
    tiles.emplace_back(6, sf::Vector2f{80.f, 20.f}, sf::Vector2f{600.f, 530.f}, StageObjectType::Death);

    // === Side walls ===
    tiles.emplace_back(7, sf::Vector2f{40.f, 600.f}, sf::Vector2f{-40.f, 0.f}, StageObjectType::Solid);
    tiles.emplace_back(8, sf::Vector2f{40.f, 600.f}, sf::Vector2f{800.f, 0.f}, StageObjectType::Solid);

    // === Spawn points (green) ===
    std::vector<sf::Vector2f> spawns = {
        sf::Vector2f{360.f, 200.f},
        sf::Vector2f{460.f, 200.f}
    };

    // === Stage setup ===
    Stage stage(1, std::move(tiles), std::move(spawns), "test");
    stage.setBounds(sf::FloatRect{{0.f, 0.f}, {800.f, 600.f}});
    stage.setWrapEdgesX(false);
    stage.setVoidTeleportY(true);
    return stage;
}
