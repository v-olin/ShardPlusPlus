/*
#pragma once

#ifdef _WINDLL
#define  __declspec(dllexport)
#else
#define  __declspec(dllimport)
#endif

// #include "Collider.h"

#include "CollisionHandler.h"

namespace Shard {
    class  Collider;

    class  ColliderRect : public Collider {
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
        void recalculate() override;
        std::optional<glm::vec2> checkCollision(ColliderRect& other) override;
        std::optional<glm::vec2> checkCollision(ColliderCircle& other) override;
        std::optional<glm::vec2> checkCollision(glm::vec2 point) override;
        void draw(SDL_Color color) override;


    private:
        bool for_minkowski = false;
    };
}
*/