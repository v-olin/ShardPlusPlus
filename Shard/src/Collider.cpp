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

    Collider::Collider(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Transform> transform)
        : x(0), y(0),
        rotate_at_offset(false),
        transform(transform),
        game_object(game_obj)
    {
    }

    Collider::Collider(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Transform> transform, float x, float y)
        : x(x), y(y),
        rotate_at_offset(false),
        transform(transform),
        game_object(game_obj)
    {
    }

}