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
    OBJECT_ID_TYPE projectileIds = 0;

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
    Player const& getPlayer(OBJECT_ID_TYPE id) const{
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
    void addPlayer(Player& player){
        players.push_back(player);
    }

    void addPlayer(Player&& player){
        players.push_back(player);
    }

    std::vector<Projectile> &getProjectiles(){
        return projectiles;
    }
    
    Projectile &getProjectile(OBJECT_ID_TYPE id){
        for (auto& projectile: projectiles)
        {
            if(projectile.getId() == id){
                return projectile;
            }
        }
        throw std::runtime_error("projectile not found");
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

    OBJECT_ID_TYPE getProjectileIds() const { return projectileIds; }

    void setProjectileIds(OBJECT_ID_TYPE i) { projectileIds = i; }


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