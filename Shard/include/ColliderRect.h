#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include "Collider.h"
#include "CollisionHandler.h"
#include "ColliderCircle.h"
#include "Transform.h"

namespace Shard {
    class SHARD_API ColliderRect : public Collider {
    public:
        float width, height, base_width, base_height;
        float left, right, top, bottom;

        ColliderRect();
        ColliderRect(CollisionHandler* game_obj, Transform* transform);
        ColliderRect(CollisionHandler* game_obj, Transform* transform, float x, float y, float width, float height);

        void calculateBoundingBox();
        ColliderRect calculateMinkowskiDifference(ColliderRect& other);
        glm::vec2 calculatePenetration(glm::vec2 point);

        // inherited from Collider
        void recalculate();
        glm::vec2 checkCollision(ColliderRect& other) override;
        glm::vec2 checkCollision(ColliderCircle& other) override;
        glm::vec2 checkCollision(glm::vec2 point) override;
        void draw(SDL_Color color) override;


    private:
        bool for_minkowski = false;
    };
}