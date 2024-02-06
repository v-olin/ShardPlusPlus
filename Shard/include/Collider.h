#pragma once

#include <string>
#include <SDL.h>
#include <typeinfo>
#include <optional>
#include <memory>

#include "CollisionHandler.h"
#include "Transform.h"

namespace Shard {
    class ColliderCircle;
    class ColliderRect;

    class Collider : public std::enable_shared_from_this<Collider> {
    public:
        float x, y;
        glm::vec2 box_top_left{};
        glm::vec2 box_bottom_right{};
        //glm::vec2 min_and_max_x;
        //glm::vec2 min_and_max_y;

       /* glm::vec2 top_left;
        glm::vec2 bottom_right;*/

        bool rotate_at_offset;
        std::shared_ptr<Transform> transform;
        std::shared_ptr<CollisionHandler> game_object;

        Collider();
        Collider(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Transform> transform);
        Collider(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Transform> transform, float x, float y);

        virtual void recalculate() = 0;
        virtual std::optional<glm::vec2> checkCollision(glm::vec2 point) = 0;
        virtual std::optional<glm::vec2> checkCollision(std::shared_ptr<Collider> c) = 0;
        virtual void draw(SDL_Color color) = 0;

    protected:
        bool from_trans{false};
    };

}
