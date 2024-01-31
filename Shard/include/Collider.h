#pragma once

#include <string>
#include <SDL.h>
#include <typeinfo>
#include <optional>

#include "CollisionHandler.h"
#include "Transform.h"

namespace Shard {
    class ColliderCircle;
    class ColliderRect;

    class  Collider {
    public:
        float x, y;
        glm::vec2 box_top_left{};
        glm::vec2 box_bottom_right{};
        glm::vec2 min_and_max_x;
        glm::vec2 min_and_max_y;

        glm::vec2 top_left;
        glm::vec2 bottom_right;

        bool rotate_at_offset;
        Transform* transform;
        CollisionHandler* game_object;

        Collider();
        Collider(CollisionHandler* game_obj, Transform* transform);
        Collider(CollisionHandler* game_obj, Transform* transform, float x, float y);

        virtual void recalculate() = 0;
        virtual std::optional<glm::vec2> checkCollision(glm::vec2 point) = 0;
        virtual std::optional<glm::vec2> checkCollision(Collider* c) = 0;
        virtual void draw(SDL_Color color) = 0;

    protected:
        bool from_trans{false};

    };

}
