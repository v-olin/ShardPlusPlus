#include "Collider.h"

namespace Shard {
	class ColliderRect;

	class  ColliderCircle : public Collider {
	public:
		float rad;
		float x_off, y_off;

		ColliderCircle();
		ColliderCircle(CollisionHandler* game_obj, Transform* transform);
		ColliderCircle(CollisionHandler* game_obj, Transform* transform, float x, float y, float rad);

		void calculateBoundingBox();

		// inherited from Collider
		void recalculate() override;
		std::optional<glm::vec2> checkCollision(glm::vec2 point) override;
		std::optional<glm::vec2> checkCollision(Collider* c) override;
		void draw(SDL_Color color) override;

		// internal colliding check
		std::optional<glm::vec2> checkCollision(ColliderRect* other);
		std::optional<glm::vec2> checkCollision(ColliderCircle* other);
	};

}
