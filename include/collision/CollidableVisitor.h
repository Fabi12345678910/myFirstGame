#pragma once
#include <iostream>

class Collidable;
class StageObject;
class Player;

class CollidableVisitor{
public:
    virtual void visit(Collidable& c) = 0;
    virtual void visit(StageObject& c) {this->visit((Collidable&) c);};
    virtual void visit(Player& c) {this->visit((Collidable&) c);};
};