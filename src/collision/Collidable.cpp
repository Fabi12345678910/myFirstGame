#include <collision/Collidable.h>
#include <collision/CollidableVisitor.h>
#include <collision/FirstCollidableVisitor.h>

void Collidable::onCollision(Collidable& c1, Collidable& c2) {
    FirstCollidableVisitor firstCollider;
    c1.accept(firstCollider);
    c2.accept(*firstCollider.nextVisitor);
}

void Collidable::secureReverseCollision(Collidable& c1, Collidable& c2) {
    static bool isReversing = false;
    if (isReversing) throw std::runtime_error("collision loop detected");

    isReversing = true;
    onCollision(c2, c1);
    isReversing = false;
}