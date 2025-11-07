#include <iostream>
#include <SFML/System.hpp>
#include <list>
class CollidableVisitor;
class Collidable;
class Player;
class Projectile;
class Laser;
class Platform;

class CollidableVisitor{
public:
    virtual void visit(Collidable&) = 0;
    virtual void visit(Platform& c) {this->visit((Collidable&) c);};
    virtual void visit(Player& c) {this->visit((Collidable&) c);};
    virtual void visit(Laser& c) {this->visit((Collidable&) c);};
    virtual void visit(Projectile& c) {this->visit((Collidable&) c);};
    
};

void onCollision(Collidable& c1, Collidable& c2);

//calls the collision in reverse order and ensures that there is no collision loop

void secureReverseCollision(Collidable& c1, Collidable& c2){
    static bool isReversing = false;
    if(isReversing){throw std::runtime_error("collision loop detected");};
    
    isReversing = true;
    onCollision(c2, c1);
    isReversing = false;
}

class Collidable{
public:
    virtual void accept(CollidableVisitor&) = 0;
};

class Player : public Collidable{
public:
    virtual void accept(CollidableVisitor& v) override{
        v.visit(*this);
    }
};
class Projectile : public Collidable{
public:
    virtual void accept(CollidableVisitor& v) override{
        v.visit(*this);
    }
};
class Laser : public Projectile{
public:
    virtual void accept(CollidableVisitor& v) override{
        v.visit(*this);
    }
};
class Platform : public Collidable{
public:
    virtual void accept(CollidableVisitor& v) override{
        v.visit(*this);
    }
};

class GenericCollidableVisitor : public CollidableVisitor{
private:
    Collidable& c;
public:
    GenericCollidableVisitor(Collidable& c):c(c){};
    virtual void visit(Collidable& ) override{
        std::cout << "generic collidable collided with generic collidable\n";
    }
};

class PlayerCollidableVisitor : public CollidableVisitor{
private:
    Player& player;
public:
    PlayerCollidableVisitor(Player& p): player(p){}
    virtual void visit(Collidable& c) override{
        secureReverseCollision(player, c);
    }
    virtual void visit(Player& player2) override{
        std::cout << "player collided with player\n";
    }
};

class PlatformCollidableVisitor : public CollidableVisitor{
private:
    Platform& platform;
public:
    PlatformCollidableVisitor(Platform& p):platform(p){};
    virtual void visit(Collidable& c) override{
        secureReverseCollision(platform, c);
    }

    virtual void visit(Player& player) override{
        std::cout << "platform collided with player\n";
    }
    virtual void visit(Platform& platform2){
        std::cout << "platform collided with platform\n";
    }
};

class ProjectileCollidableVisitor : public CollidableVisitor{
private:
    Projectile& projectile;
public:
    ProjectileCollidableVisitor(Projectile& projectile):projectile(projectile){};
    virtual void visit(Collidable& c) override{
        secureReverseCollision(projectile, c);
    }
    virtual void visit(Platform& p) override{
        std::cout << "projectile hit platform\n";
    }
    virtual void visit(Projectile& p) override{
        std::cout << "projectile hit projectile\n";
    }
    virtual void visit(Player& p) override{
        std::cout << "projectile hit player\n";
    }
};

class LaserCollidableVisitor : public ProjectileCollidableVisitor{
private:
    Laser& laser;
public:
    LaserCollidableVisitor(Laser& laser):laser(laser), ProjectileCollidableVisitor(laser){};
    virtual void visit(Player& p) override{
        std::cout << "laser hit player\n";
    }
    virtual void visit(Laser& laser2){
        std::cout << "laser hit laser\n";
    }
};

//gets created only for the purpose of deciding which CollidableVisitor to then create
class FirstCollidableVisitor: public CollidableVisitor{
public:
    CollidableVisitor* nextVisitor;
    virtual void visit(Collidable& c) override{
        nextVisitor = new GenericCollidableVisitor(c);
    }
    virtual void visit(Player& p) override{
        nextVisitor = new PlayerCollidableVisitor(p);
    }
    virtual void visit(Platform& p) override{
        nextVisitor = new PlatformCollidableVisitor(p);
    }
    virtual void visit(Projectile& p) override{
        nextVisitor = new ProjectileCollidableVisitor(p);
    }
    virtual void visit(Laser& l) override{
        nextVisitor = new LaserCollidableVisitor(l);
    }
};

void onCollision(Collidable& c1, Collidable& c2){
    FirstCollidableVisitor firstCollider;
    c1.accept(firstCollider);
    c2.accept(*firstCollider.nextVisitor);
}

int main(int argc, char const *argv[])
{
    Player player;
    Projectile projectile;
    Platform platform;
    Laser laser;

    onCollision(player,platform);
    onCollision(platform,player);
    onCollision(platform, platform);
    onCollision(projectile, platform);
    onCollision(platform, projectile);
    onCollision(laser, platform);
    onCollision(platform, laser);
    onCollision(projectile, player);
    onCollision(player, projectile);
    onCollision(player, laser);
    onCollision(laser, player);
    onCollision(laser, laser);
    onCollision(laser, projectile);
    onCollision(projectile, laser);

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