#pragma once
#include <collision/CollidableVisitor.h>
#include <collision/Collidable.h>

class GenericCollidableVisitor : public CollidableVisitor{
private:
    Collidable& c;
public:
    GenericCollidableVisitor(Collidable& c):c(c){};
    virtual void visit(Collidable& ) override{
        std::cout << "generic collidable collided with generic collidable\n";
    }
};
