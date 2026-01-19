#pragma once
#include "Config.h"
#include "GameState.h"
#include "SFML/Network/Packet.hpp"
#include "UpdateInfo.h"
#include "plog/Log.h"

struct playerUpdateInfo{
    OBJECT_ID_TYPE id;
    sf::Vector2f position;
    sf::Vector2f velocity;
    bool facing;
    float gravity;
    float speed;
    float health;
    bool readyToPlay;
    int16_t  projectileCooldown;
    playerUpdateInfo(){};
    playerUpdateInfo(Player &p){
        id = p.getId();
        position = p.getPosition();
        velocity = p.getVelocity();
        facing = p.getFacing();
        gravity = p.getGravity();
        speed = p.getSpeed();
        health = p.getHealth();
        readyToPlay = p.getReadyToPlay();
        projectileCooldown = p.getProjectileCooldown();
    }
    void applyUpdate(Player &player){
        PLOG_VERBOSE_IF(debugClientGameStore) << "update player position to " << position.x << ":" << position.y;
        player.setPosition(position);
        player.setVelocity(velocity);
        player.setFacing(facing);
        player.setGravity(gravity);
        player.setSpeed(speed);
        player.setHealth(health);
        player.setReadyToPlay(readyToPlay);
        player.setProjectileCooldown(projectileCooldown);
    }
};

struct projectileUpdateInfo{
    OBJECT_ID_TYPE id;
    sf::Vector2f position;
    sf::Vector2f velocity;
    float speed;
    projectileUpdateInfo(){};
    projectileUpdateInfo(Projectile &p){
        id = p.getId();
        position = p.getPosition();
        velocity = p.getVelocity();
        speed = p.getSpeed();
    }
    void applyUpdate(Projectile& projectile){
        projectile.setPosition(position);
        projectile.setVelocity(velocity);
        projectile.setSpeed(speed);
    }
};

struct gsUpdateInfo : public UpdateInfo{
    TICK_TYPE latestIncludedPInput;
    std::vector<playerUpdateInfo> playerInfos;
    std::vector<projectileUpdateInfo> projectileInfos;
    virtual bool applyUpdate(GameStateUpdater& gsUpdater, GameState& gameState) override{
        PLOG_VERBOSE_IF(debugClientGameStore) << "gsUpdate info contains " << playerInfos.size() << " players";
        for (auto& playerInfo : playerInfos)
        {
            try
            {
                playerInfo.applyUpdate(gameState.getPlayer(playerInfo.id));
            }
            catch(const std::runtime_error& e)
            {
                PLOG_DEBUG_IF(debugClientGameStore) << "could not update player because he was not found";
                Player newPlayer(playerInfo.id, sf::Vector2f(40.f, 40.f), playerInfo.position);
                playerInfo.applyUpdate(newPlayer);
                gsUpdater.addPlayer(newPlayer);
            }
        }

        for (auto& projectileInfo : projectileInfos)
        {
            try
            {
                projectileInfo.applyUpdate(gameState.getProjectile(projectileInfo.id));
            }
            catch(const std::runtime_error& e)
            {
                Projectile proj(projectileInfo.id, {20.f,20.f}, projectileInfo.position);
                gameState.addProjectile(proj);
                projectileInfo.applyUpdate(gameState.getProjectile(projectileInfo.id));
                PLOG_DEBUG_IF(debugClientGameStore) << "did not found projectile with id "<< projectileInfo.id <<" for update, spawned instead";
            }
        }
        return true;
    };
};

inline sf::Packet& operator <<(sf::Packet& packet, const sf::Vector2f& v)
{
    return packet << v.x << v.y;
}

inline sf::Packet& operator >>(sf::Packet& packet, sf::Vector2f& v)
{
    return packet >> v.x >> v.y;
}

inline sf::Packet& operator <<(sf::Packet& packet, const playerUpdateInfo& p)
{
    return packet 
        << p.id
        << p.position
        << p.velocity
        << p.facing
        << p.gravity
        << p.speed
        << p.health
        << p.readyToPlay
        << p.projectileCooldown;
}

inline sf::Packet& operator >>(sf::Packet& packet, playerUpdateInfo& p)
{
    return packet
        >> p.id
        >> p.position
        >> p.velocity
        >> p.facing
        >> p.gravity
        >> p.speed
        >> p.health
        >> p.readyToPlay
        >> p.projectileCooldown;
}

inline sf::Packet& operator <<(sf::Packet& packet, const projectileUpdateInfo& pr)
{
    return packet
        << pr.id
        << pr.position
        << pr.velocity
        << pr.speed;
}

inline sf::Packet& operator >>(sf::Packet& packet, projectileUpdateInfo& pr)
{
    return packet
        >> pr.id
        >> pr.position
        >> pr.velocity
        >> pr.speed;
}

inline sf::Packet& operator <<(sf::Packet& packet, const gsUpdateInfo& gs)
{
    // Players
    packet << gs.latestIncludedPInput;
    packet << static_cast<uint32_t>(gs.playerInfos.size());
    for (const auto& p : gs.playerInfos){
        packet << p;
    }

    // Projectiles
    packet << static_cast<uint32_t>(gs.projectileInfos.size());
    for (const auto& pr : gs.projectileInfos){
        packet << pr;
    }

    return packet;
}

inline sf::Packet& operator >>(sf::Packet& packet, gsUpdateInfo& gs)
{
    packet >> gs.latestIncludedPInput;
    uint32_t count;
    // Players
    packet >> count;
    gs.playerInfos.resize(count);
    for (uint32_t i = 0; i < count; i++){
        packet >> gs.playerInfos[i];
    }
    // Projectiles
    packet >> count;
    gs.projectileInfos.resize(count);
    for (uint32_t i = 0; i < count; i++){
        packet >> gs.projectileInfos[i];
    }
        
    return packet;
}
