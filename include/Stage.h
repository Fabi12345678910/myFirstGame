#pragma once
#include <SFML/Graphics.hpp>
#include "StageObject.h"
#include <vector>
#include <cstdint>

class Stage {
private:
    std::vector<StageObject> stageObjects;
    std::vector<sf::Vector2f> spawnPoints;
    int16_t stageId{0};

    bool wrapEdgesX{false};     // if true, exiting left/right wraps to the other side
    bool voidTeleportY{true};   // if true, falling below bottom teleports to top (not death)
    sf::FloatRect bounds{sf::Vector2f{0.f, 0.f}, sf::Vector2f{800.f, 600.f}}; // default arena

public:
    Stage(int16_t stageId,
          std::vector<StageObject> stageObjects,
          std::vector<sf::Vector2f> spawnPoints)
        : stageObjects(std::move(stageObjects)),
          spawnPoints(std::move(spawnPoints)),
          stageId(stageId) {}

    Stage() = default;

    StageObject& getStageObject(OBJECT_ID_TYPE id){
        for (auto& stageObject : stageObjects)
            if (stageObject.getId() == id) return stageObject;
        throw std::runtime_error("stageObject not found");
    }

    const std::vector<StageObject>& getStageObjects() const { return stageObjects; }
    std::vector<StageObject>&       getStageObjects()       { return stageObjects; }

    std::vector<sf::Vector2f>&       getSpawnPoints()       { return spawnPoints; }
    const std::vector<sf::Vector2f>& getSpawnPoints() const { return spawnPoints; }

    int16_t getStageId() const { return stageId; }

    bool getWrapEdgesX() const { return wrapEdgesX; }
    void setWrapEdgesX(bool v) { wrapEdgesX = v; }

    bool getVoidTeleportY() const { return voidTeleportY; }
    void setVoidTeleportY(bool v) { voidTeleportY = v; }

    const sf::FloatRect& getBounds() const { return bounds; }
    void setBounds(const sf::FloatRect& b) { bounds = b; }
};
