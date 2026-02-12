#include "StageManager.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

Stage StageManager::loadStage(int16_t stageId)
{
    std::ifstream file(resolveStagePath(stageId));
    if (!file)
        throw std::runtime_error("Stage file not found");

    json j;
    file >> j;

    std::vector<StageObject> objects;
    std::vector<sf::Vector2f> spawns;

    for (auto& s : j["spawnPoints"])
        spawns.emplace_back(s["x"], s["y"]);

    for (auto& o : j["objects"])
    {
        int id = o["id"];
        sf::Vector2f size{ o["size"]["w"], o["size"]["h"] };
        sf::Vector2f position{ o["position"]["x"], o["position"]["y"] };
        std::string typeStr = o["type"];

        StageObjectType type;
        if (typeStr == "Solid")       type = StageObjectType::Solid;
        else if (typeStr == "HalfSolid") type = StageObjectType::HalfSolid;
        else if (typeStr == "JumpPad")   type = StageObjectType::JumpPad;
        else if (typeStr == "Death")     type = StageObjectType::Death;
        else throw std::runtime_error("Unknown StageObject type: " + typeStr);

        objects.emplace_back(id, size, position, type);
    }

    Stage stage(
        stageId,
        std::move(objects),
        std::move(spawns),
        j["name"].get<std::string>()
    );

    stage.setWrapEdgesX(j["wrapEdgesX"]);
    stage.setVoidTeleportY(j["voidTeleportY"]);
    stage.setBounds(sf::FloatRect(sf::Vector2f(
        j["bounds"]["x"].get<float>(),
        j["bounds"]["y"].get<float>()),sf::Vector2f(
        j["bounds"]["w"].get<float>(),
        j["bounds"]["h"].get<float>())
    ));

    return stage;
}

std::vector<std::pair<int16_t, std::string>> StageManager::loadStageList()
{
    std::vector<std::pair<int16_t, std::string>> stages;

    for (const auto& entry : std::filesystem::directory_iterator("../assets/stages"))
    {
        if (entry.path().extension() != ".json")
            continue;

        std::ifstream file(entry.path());
        if (!file)
            continue;

        json j;
        file >> j;

        if (!j.contains("stageId") || !j.contains("name"))
            continue;

        int16_t id = j["stageId"];
        std::string name = j["name"];

        stages.emplace_back(id, name);
    }

    std::sort(stages.begin(), stages.end(),
              [](auto& a, auto& b) { return a.first < b.first; });

    return stages;
}


std::string StageManager::resolveStagePath(int16_t stageId)
{
    return "../assets/stages/" + std::to_string(stageId) + ".json";
}
