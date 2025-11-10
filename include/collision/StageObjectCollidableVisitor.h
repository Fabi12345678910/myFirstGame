#pragma once
#include <collision/CollidableVisitor.h>
#include <StageObject.h>
#include <collision/Collidable.h>

class StageObjectCollidableVisitor : public CollidableVisitor{
private:
    StageObject& stageObject;
public:
    StageObjectCollidableVisitor(StageObject& p):stageObject(p){};
    virtual void visit(Collidable& c) override{
        Collidable::secureReverseCollision(stageObject, c);
    }

    virtual void visit(Player& player) override{
        std::cout << "stageObject collided with player\n";
    }
    virtual void visit(StageObject& stageObject2){
        std::cout << "stageObject collided with platform\n";
    }
};
