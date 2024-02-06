#include "Collider.h"

namespace Shard {
    class ColliderCircle;

    class  ColliderRect : public Collider {
    public:
        float width, height, base_width, base_height;
        float getLeft()    { return box_top_left.x; }
        float getRight()  { return box_bottom_right.x; }
        float getTop()    { return box_top_left.y; }
        float getBottom() { return box_bottom_right.y; }

        ColliderRect();
        ColliderRect(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Transform> transform);
        ColliderRect(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Transform> transform, float x, float y, float width, float height);

        void calculateBoundingBox();
        ColliderRect calculateMinkowskiDifference(ColliderRect& other);
        glm::vec2 calculatePenetration(glm::vec2 point);

        // inherited from Collider
        void recalculate() override;
        std::optional<glm::vec2> checkCollision(glm::vec2 point) override;
        std::optional<glm::vec2> checkCollision(std::shared_ptr<Collider> c) override;
        void draw(SDL_Color color) override;

        //internal colliding check
        std::optional<glm::vec2> checkCollision(std::shared_ptr<ColliderRect> other);
        std::optional<glm::vec2> checkCollision(std::shared_ptr<ColliderCircle> other);

    private:
        bool for_minkowski = false;

    };

}
