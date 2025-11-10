#pragma once
#include <collision/CollidableVisitor.h>

enum class CollisionKind {
    None,
    Solid,      // This is the full solid block, can't pass through on any side
    SemiSolid,  // This one is the one where u can jump up from below but still colide from top/sides
    Hazard,     // touching this tile kills the player
    Trigger     // different kind of stuff gets triggered, like a jump pad, if u stap on it it can launch you up but can also be used for other trigger stuff
};

class Collidable {
public:
    virtual void accept(CollidableVisitor&) = 0;
    static void onCollision(Collidable& c1, Collidable& c2);
    static void secureReverseCollision(Collidable& c1, Collidable& c2);
};
