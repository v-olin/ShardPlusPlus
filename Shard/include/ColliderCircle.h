#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include "Collider.h"

namespace Shard {
	class SHARD_API ColliderCircle : public Collider {
	public:
		float rad;
		float x_off, y_off;

		//ColliderCircle();
		ColliderCircle(CollisionHandler* game_obj, Transform* transform);
		ColliderCircle(CollisionHandler* game_obj, Transform* transform, float x, float y, float rad);

		void calculateBoundingBox();

		// inherited from Collider
		void recalculate() override;
		glm::vec2 checkCollision(ColliderRect& other) override;
		glm::vec2 checkCollision(glm::vec2 point) override;
		glm::vec2 checkCollision(ColliderCircle& other) override;
		void draw(SDL_Color color) override;
	};
}