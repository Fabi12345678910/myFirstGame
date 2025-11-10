#pragma once
#include <collision/CollidableVisitor.h>
#include <collision/GenericCollidableVisitor.h>
#include <collision/PlayerCollidableVisitor.h>
#include <collision/StageObjectCollidableVisitor.h>

class FirstCollidableVisitor: public CollidableVisitor{
public:
    CollidableVisitor* nextVisitor;
    virtual void visit(Collidable& c) override{
        nextVisitor = new GenericCollidableVisitor(c);
    }
    virtual void visit(Player& p) override{
        nextVisitor = new PlayerCollidableVisitor(p);
    }
    virtual void visit(StageObject& p) override{
        nextVisitor = new StageObjectCollidableVisitor(p);
    }
};
