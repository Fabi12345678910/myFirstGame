#pragma once
#include <collision/CollidableVisitor.h>

class Collidable;
class Player;

class PlayerCollidableVisitor : public CollidableVisitor{
private:
    Player& player;
public:
    PlayerCollidableVisitor(Player& p): player(p){}
    virtual void visit(Collidable& c) override;
    virtual void visit(Player& player2) override;
};

