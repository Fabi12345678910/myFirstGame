#pragma once
#include "Player.h"
#include "Stage.h"

class GameState {
private:
    std::vector<Player> players;
    std::vector<GameObject> gameObjects;
    Stage stage;

    Player &getPlayer(int id){
        for (auto& player:players)
        {
            if(player.getId() == id){
                return player;
            }
        }
        throw std::runtime_error("player not found");
    }
    
    GameObject& getGameObject(int id){

        //return GameObject... somehow
    }
    Stage& getStage(){
        return stage;
    }
    void setStage(Stage& stage){
        this->stage = stage;
    }
};