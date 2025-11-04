#pragma once
#include "StageObject.h"

class Stage {
private:
    std::vector<StageObject> stageObjects;
    std::vector<sf::Vector2f> spawnPoints;
    int16_t stageId;

public:
    Stage(int16_t stageId, std::vector<StageObject> stageObjects, std::vector<sf::Vector2f> spawnPoints){
        this->stageId = stageId;
        this->stageObjects = stageObjects;
        this->spawnPoints = spawnPoints;
    }

    StageObject& getStageObject(OBJECT_ID_TYPE id){
        for (auto& stageObject:stageObjects)
        {
            if(stageObject.getId() == id){
                return stageObject;
            }
        }
        throw std::runtime_error("stageObject not found");
    }

    std::vector<StageObject> const & getStageObjects(){
        return stageObjects;
    }

    std::vector<sf::Vector2f>& getSpawnPoints(){
        return spawnPoints;
    }

    int16_t getStageId(){return stageId;};
    Stage(){};
    
};