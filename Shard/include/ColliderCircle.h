#include "Collider.h"

#include <memory>

namespace Shard {
	class ColliderRect;

	class  ColliderCircle : public Collider {
	public:
		float rad;
		float x_off, y_off;

		ColliderCircle();
		ColliderCircle(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Transform> transform);
		ColliderCircle(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Transform> transform, float x, float y, float rad);

		void calculateBoundingBox();

		// inherited from Collider
		void recalculate() override;
		std::optional<glm::vec2> checkCollision(glm::vec2 point) override;
		std::optional<glm::vec2> checkCollision(std::shared_ptr<Collider> c) override;
		void draw(SDL_Color color) override;

		// internal colliding check
		std::optional<glm::vec2> checkCollision(std::shared_ptr<ColliderRect> other);
		std::optional<glm::vec2> checkCollision(std::shared_ptr<ColliderCircle> other);
	};

}
