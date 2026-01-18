#pragma once
#include "Player.h"
#include "Stage.h"
#include "Projectile.h"
#include "Types.h"
#include <map>

enum gameState{RUNNING, LOBBY, MAP_SELECT, LOADING, STARTING, END_OF_ROUND};

class GameState {
private:
    gameState currentGameState = LOBBY;
    TICK_TYPE gameStartTick;
    std::map<OBJECT_ID_TYPE, Player> playerrs;
    std::vector<Player> players;
    std::vector<Projectile> projectiles;
    std::vector<GameObject> gameObjects;
    Stage stage;
    OBJECT_ID_TYPE projectileIds = 0;

public:
    Player &getPlayer(OBJECT_ID_TYPE id){
        if(playerrs.count(id)){
            return playerrs.at(id);
        }else{
            throw std::runtime_error("player not found");
        }
    }
    Player const& getPlayer(OBJECT_ID_TYPE id) const{
        if(playerrs.count(id)){
            return playerrs.at(id);
        }else{
            throw std::runtime_error("player not found");
        }
    }

    void removePlayer(OBJECT_ID_TYPE id){
        auto it = playerrs.find(id);

        // Deleting the key-value pair using erase()
        if (it != playerrs.end()) {
            playerrs.erase(it);
        }
    }
    size_t getPlayerCount() const{
        return playerrs.size();
    }

    std::map<OBJECT_ID_TYPE, Player>::const_iterator getPlayersBegin() const{
        return playerrs.begin();
    }
    std::map<OBJECT_ID_TYPE, Player>::iterator getPlayersBegin(){
        return playerrs.begin();
    }

    std::map<OBJECT_ID_TYPE, Player>::const_iterator getPlayersEnd() const{
        return playerrs.end();
    }
    std::map<OBJECT_ID_TYPE, Player>::iterator getPlayersEnd(){
        return playerrs.end();
    }

    void addPlayer(Player& player){
        playerrs.insert({player.getId(), player});
    }

    void addPlayer(Player&& player){
        playerrs.insert({player.getId(), player});
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

    void removeProjectile(OBJECT_ID_TYPE id){
        for (size_t i = 0; i < projectiles.size(); i++)
        {
            if(projectiles[i].getId() == id){
                projectiles.erase(projectiles.begin()+i);
                return;
            }
        }
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

    gameState getGameState() const {
        return currentGameState;
    }
    void setGameState(gameState gameState) {
        currentGameState = gameState;
    }
    TICK_TYPE getGameStartTick() const {
        return gameStartTick;
    }
    void setGameStartTick(TICK_TYPE startTick) {
        gameStartTick = startTick;
    }
    GameState(){};

    static std::string toString(gameState state)
    {
        switch (state)
        {
            case gameState::LOBBY:   return "LOBBY";
            case gameState::MAP_SELECT:return "MAP_SELECT";
            case gameState::RUNNING:   return "RUNNING";
            case gameState::LOADING:   return "LOADING";
            case gameState::STARTING:  return "STARTING";
            default:                  return "UNKNOWN";
        }
    }
};