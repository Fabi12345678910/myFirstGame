#include <iostream>
#include <SFML/System.hpp>
#include <list>

#define TRIGGER_COLLISION     virtual std::list<GameUpdate> triggerCollision(Collider& c) override{ \
        return c.handleCollision(*this); \
    }
#define HANDLE_COLLISION(collidableType)     virtual std::list<GameUpdate> handleCollision(collidableType& c) override{\
        return globalHandleCollision(*this, c);\
    }
#define HANDLE_COLLISION_REVERSED(collidableType)     virtual std::list<GameUpdate> handleCollision(collidableType& c) override{\
        return globalHandleCollision(c, *this);\
    }

class Collidable;

class Platform;
class Player;
class Projectile;
class Laser;

struct CollisionDetails{
    sf::Vector2f vectorCollider;
    sf::Vector2f vectorCollidable;
    sf::Vector2f collisionPoint;
    //... oder mehr infos, oder weniger infos
};

struct GameUpdate{
    int32_t type;
    int32_t value;
    void apply(){};
};

std::list<GameUpdate> globalHandleCollision(Collider &pl, Collidable&c);

std::list<GameUpdate> globalHandleCollision(Player &pl, Player&c);
std::list<GameUpdate> globalHandleCollision(Player &pl, Projectile&c);
std::list<GameUpdate> globalHandleCollision(Player &pl, Platform&c);

std::list<GameUpdate> globalHandleCollision(Projectile &pl, Collidable&c);
std::list<GameUpdate> globalHandleCollision(Projectile &pl, Projectile&c);
std::list<GameUpdate> globalHandleCollision(Projectile &pl, Platform&c);

std::list<GameUpdate> globalHandleCollision(Laser &pl, Collidable&c);
std::list<GameUpdate> globalHandleCollision(Laser &pl, Player&c);
#define HANDLE_COLLISION_DEFAULT_COLLIDABLE(collidableType)virtual std::list<GameUpdate> handleCollision(collidableType& c) {std::cout << "defaulting to Collidable handler, "; return this->handleCollision((Collidable&)c);}
class Collider{
    public:
        virtual std::list<GameUpdate> handleCollision(Collidable& c) = 0;
        HANDLE_COLLISION_DEFAULT_COLLIDABLE(Platform);
        HANDLE_COLLISION_DEFAULT_COLLIDABLE(Player);
        HANDLE_COLLISION_DEFAULT_COLLIDABLE(Projectile);
        HANDLE_COLLISION_DEFAULT_COLLIDABLE(Laser);
};


class Collidable{
public:
    virtual std::list<GameUpdate> triggerCollision(Collider& c) = 0;
};

class Platform: public Collidable{
public:
    TRIGGER_COLLISION;
};

class Projectile: public Collider, public Collidable{
public:
    TRIGGER_COLLISION;
    HANDLE_COLLISION(Collidable);
    HANDLE_COLLISION(Platform);
    HANDLE_COLLISION_REVERSED(Player);
};

class Player: public Collidable, public Collider{
public:
    int getHealth(){return 100;};
    TRIGGER_COLLISION;
    HANDLE_COLLISION(Collidable);
    HANDLE_COLLISION(Projectile);
    HANDLE_COLLISION(Platform);
    HANDLE_COLLISION_REVERSED(Laser);
};

class Laser: public Collidable, public Collider{
    TRIGGER_COLLISION;
    HANDLE_COLLISION(Collidable);
    HANDLE_COLLISION(Player);
};

std::list<GameUpdate> globalHandleCollision(Collider &pl, Collidable&c){
    std::cout << "general collision\n";
    return std::list<GameUpdate>();
}

std::list<GameUpdate> globalHandleCollision(Player &pl, Projectile&p){
    std::cout << "player got hit by projectile\n";
    std::cout << "players health: " << pl.getHealth() << '\n';
    return std::list<GameUpdate>();
}

std::list<GameUpdate> globalHandleCollision(Player &pl, Player&p){
    std::cout << "player collided with player\n";
    return std::list<GameUpdate>();
}

std::list<GameUpdate> globalHandleCollision(Player &pl, Platform&p){
    std::cout << "player collided with platform\n";
    return std::list<GameUpdate>();
}

std::list<GameUpdate> globalHandleCollision(Laser &pl, Collidable&c){
    std::cout << "laser hit something general, removing laser\n"; 
    return std::list<GameUpdate>();
}
std::list<GameUpdate> globalHandleCollision(Laser &pl, Player&c){
    std::cout << "laser hit player, removing laser and damaging player\n";
    return std::list<GameUpdate>();
}

std::list<GameUpdate> globalHandleCollision(Projectile &pl, Collidable&c){
    std::cout << "projectile collided with unknown collidable\n";
    return std::list<GameUpdate>();
}
std::list<GameUpdate> globalHandleCollision(Projectile &pl, Projectile&c){
    std::cout << "projectile collided with projectile\n";
    return std::list<GameUpdate>();
}
std::list<GameUpdate> globalHandleCollision(Projectile &pl, Platform&c){
    std::cout << "projectile collided with platform\n";
    return std::list<GameUpdate>();
}

int main(int argc, char const *argv[])
{
    Player player;
    Projectile arrow;
    Platform platform;
    Laser laser;

    platform.triggerCollision(player);
    platform.triggerCollision(arrow);
    player.triggerCollision(arrow);

    platform.triggerCollision(laser);
    arrow.triggerCollision(laser);
    player.triggerCollision(laser);
    return 0;
}

/*
adding a new collider 'newCollider':

1. predefine Class(only write class newCollider)
2. add virtual method to interface Collider (simplified with makro)

3. adding a default collisionHandler:
3.1. use makro HANDLE_COLLISION(Collidable) to overwrite method in class newCollider
3.2. define globalHandleCollision(newCollider&, Collidable&)
3.3. implement globalHandleCollision(newCollider&, Collidable&)

4. (optional) for each Collidable 'presentCollidable' if special collisionHandling is required:
4.1. use HANDLE_COLLISION(presentCollidable) to overwrite method in class newCollider
4.2. if newCollider is also a Collidable: use HANDLE_COLLISION_REVERSED(newCollider) in class presentCollidable
4.3. define globalHandleCollision(newCollider&, presentCollidable&)
4.4. implement globalHandleCollision(newCollider&, presentCollidable&)
*/