#pragma once
#include "Collidable.h"
#include "GameObject.h"
#include <SFML/Graphics.hpp>

enum class StageObjectType {
    Solid,         // This is the full solid block, can't pass through on any side
    HalfSolid,     // This one is the one where u can jump up from below but still colide from top/sides
    Death,         // touching this tile kills the player
    MovingPlatform,// solid but with motion
    JumpPad        // trigger: vertical launch pad
};

class StageObject : public Collidable, public GameObject {
    StageObjectType type = StageObjectType::Solid;

public:
    StageObject(OBJECT_ID_TYPE id,
                sf::Vector2f size,
                sf::Vector2f position,
                StageObjectType t = StageObjectType::Solid)
        : GameObject(id, size, position), type(t) {}

    StageObjectType getType() const { return type; }

    CollisionKind collisionKind() const override {
        switch (type) {
            case StageObjectType::HalfSolid:      return CollisionKind::SemiSolid;
            case StageObjectType::Death:          return CollisionKind::Hazard;
            case StageObjectType::JumpPad:        return CollisionKind::Trigger;
            case StageObjectType::MovingPlatform: return CollisionKind::Solid;
            default:                               return CollisionKind::Solid;
        }
    }
};
