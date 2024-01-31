#include "Collider.h"

namespace Shard {
    class ColliderCircle;

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
        std::optional<glm::vec2> checkCollision(glm::vec2 point) override;
        std::optional<glm::vec2> checkCollision(Collider* c) override;
        void draw(SDL_Color color) override;

        //internal colliding check
        std::optional<glm::vec2> checkCollision(ColliderRect* other);
        std::optional<glm::vec2> checkCollision(ColliderCircle* other);

    private:
        bool for_minkowski = false;

    };

}
