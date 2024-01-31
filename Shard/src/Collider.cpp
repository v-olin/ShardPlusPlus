#include "Transform.h"
#include "Collider.h"
#include "Bootstrap.h"
#include "Display.h"

namespace Shard {

    Collider::Collider()
        : x(0), y(0),
        rotate_at_offset(false),
        transform(nullptr),
        game_object(nullptr) {
    }

    Collider::Collider(CollisionHandler* game_obj, Transform* transform)
        : x(0), y(0),
        rotate_at_offset(false),
        transform(nullptr),
        game_object(nullptr)
    {
        this->x = 0;
        this->y = 0;
        this->game_object = game_obj;
        this->transform = transform;
    }

    Collider::Collider(CollisionHandler* game_obj, Transform* transform, float x, float y)
        : x(0), y(0),
        rotate_at_offset(false),
        transform(nullptr),
        game_object(nullptr)
    {
        this->x = x;
        this->y = y;
        this->game_object = game_obj;
        this->transform = transform;
    }

}