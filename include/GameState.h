#pragma once
#include "GameObject.h"
#include "Player.h"
#include "Stage.h"
#include "Projectile.h"
#include "Types.h"
#include <map>

enum gameState{RUNNING, LOBBY, MAP_SELECT, LOADING, STARTING, END_OF_ROUND};

class GameState {
private:
    typedef std::map<OBJECT_ID_TYPE, Player> PlayerMap;
    typedef std::map<OBJECT_ID_TYPE, Projectile> ProjectileMap;
    typedef std::map<OBJECT_ID_TYPE, GameObject> GameObjectMap;
    gameState currentGameState = LOBBY;
    TICK_TYPE gameStartTick;
    PlayerMap players;
    ProjectileMap projectiles;
    GameObjectMap gameObjects;
    Stage stage;
    OBJECT_ID_TYPE projectileIds = 0;

public:
    Player &getPlayer(OBJECT_ID_TYPE id){
        if(players.count(id)){
            return players.at(id);
        }else{
            throw std::runtime_error("player not found");
        }
    }
    Player const& getPlayer(OBJECT_ID_TYPE id) const{
        if(players.count(id)){
            return players.at(id);
        }else{
            throw std::runtime_error("player not found");
        }
    }

    void removePlayer(OBJECT_ID_TYPE id){
        auto it = players.find(id);

        // Deleting the key-value pair using erase()
        if (it != players.end()) {
            players.erase(it);
        }
    }
    size_t getPlayerCount() const{
        return players.size();
    }

    PlayerMap::const_iterator getPlayersBegin() const{
        return players.begin();
    }
    PlayerMap::iterator getPlayersBegin(){
        return players.begin();
    }
    PlayerMap::const_iterator getPlayersEnd() const{
        return players.end();
    }
    PlayerMap::iterator getPlayersEnd(){
        return players.end();
    }

    ProjectileMap::const_iterator getProjectilesBegin() const{
        return projectiles.begin();
    }
    ProjectileMap::iterator getProjectilesBegin(){
        return projectiles.begin();
    }
    ProjectileMap::const_iterator getProjectilesEnd() const{
        return projectiles.end();
    }
    ProjectileMap::iterator getProjectilesEnd(){
        return projectiles.end();
    }
    void clearProjectiles(){
        projectiles.clear();
    }

    GameObjectMap::const_iterator getGameObjectsBegin() const{
        return gameObjects.begin();
    }
    GameObjectMap::iterator getGameObjectsBegin(){
        return gameObjects.begin();
    }
    GameObjectMap::const_iterator getGameObjectsEnd() const{
        return gameObjects.end();
    }
    GameObjectMap::iterator getGameObjectsEnd(){
        return gameObjects.end();
    }
    void clearGameObjects(){
        gameObjects.clear();
    }

    Player& addPlayer(Player& player){
        return players.insert({player.getId(), player}).first->second;
    }

    Player& addPlayer(Player&& player){
        return players.insert({player.getId(), player}).first->second;
    }
    void clearPlayers(){
        players.clear();
    }

    Projectile &getProjectile(OBJECT_ID_TYPE id){
        if(projectiles.count(id)){
            return projectiles.at(id);
        }else{
            throw std::runtime_error("projectile not found");
        }
    }

    void removeProjectile(OBJECT_ID_TYPE id){
        auto it = projectiles.find(id);

        // Deleting the key-value pair using erase()
        if (it != projectiles.end()) {
            projectiles.erase(it);
        }
    }

    Projectile& addProjectile(Projectile& projectile){
        return projectiles.insert({projectile.getId(),projectile}).first->second;
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