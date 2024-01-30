/*
#pragma once

#ifdef _WINDLL
#define  __declspec(dllexport)
#else
#define  __declspec(dllimport)
#endif

//#include "Collider.h"

#include "CollisionHandler.h"

namespace Shard {
	class  Collider;

	class  ColliderCircle : public Collider {
	public:
		float rad;
		float x_off, y_off;

		//ColliderCircle();
		ColliderCircle(CollisionHandler* game_obj, Transform* transform);
		ColliderCircle(CollisionHandler* game_obj, Transform* transform, float x, float y, float rad);

		void calculateBoundingBox();

		// inherited from Collider
		void recalculate() override;
		std::optional<glm::vec2> checkCollision(ColliderRect& other) override;
		std::optional<glm::vec2> checkCollision(glm::vec2 point) override;
		std::optional<glm::vec2> checkCollision(ColliderCircle& other) override;
		void draw(SDL_Color color) override;
	};
}
*/