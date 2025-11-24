#pragma once
#include "Player.h"
#include "Stage.h"
#include "Projectile.h"

class GameState {
private:
    std::vector<Player> players;
    std::vector<Projectile> projectiles;
    std::vector<GameObject> gameObjects;
    Stage stage;
    int bulletIDs;

public:
    Player &getPlayer(OBJECT_ID_TYPE id){
        for (auto& player:players)
        {
            if(player.getId() == id){
                return player;
            }
        }
        throw std::runtime_error("player not found");
    }
    std::vector<Player>&getPlayers(){
        return players;
    }
    void addPlayer(Player&& player){
        players.push_back(player);
    }

    std::vector<Projectile> &getProjectiles(){
        return projectiles;
    }
    std::vector<Projectile> getActiveProjectiles() {
        std::vector<Projectile> active;
        active.reserve(projectiles.size());

        for (auto& p : projectiles) {
            if (p.getIsActive()) {
                active.push_back(p);
            }
        }
        return active;
    }

    void addProjectile(Projectile& projectile){
        projectiles.push_back(projectile);
    }

    int getBulletIDs() { return bulletIDs; }

    void setBulletIDs(int i) { bulletIDs = i; }


/*    GameObject& getGameObject(int id){

        //return GameObject... somehow
    }*/
    Stage& getStage(){
        return stage;
    }
    void setStage(Stage& stage){
        this->stage = stage;
    }
    GameState(){};
};