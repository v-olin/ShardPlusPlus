#pragma once

#include <string>
#include <typeinfo>
#include <optional>
#include <memory>
#include <vector>

#include "CollisionHandler.h"
#include "Transform.h"

namespace Shard {

    typedef struct Ray;
    class ColliderBox;

    class Collider : public std::enable_shared_from_this<Collider> {
    public:
        
        bool rotate_at_offset{ false };
        std::shared_ptr<Transform> transform{ nullptr };
        std::shared_ptr<CollisionHandler> game_object{ nullptr };

        Collider() = default;
        Collider(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Transform> transform);

        virtual void recalculateBoundingBox() = 0;

        virtual std::optional<glm::vec3> checkCollision(Ray& ray) = 0;
        virtual std::vector<glm::vec2> getMinMaxDims() = 0;

        virtual void draw(glm::vec3 color) = 0;
        
    protected:
        bool from_trans{false};
    };

}
