/*
#include "ColliderCircle.h"

#include "Collider.h"
#include "ColliderRect.h"
#include "Transform.h"

namespace Shard {
	ColliderCircle::ColliderCircle(CollisionHandler* game_obj, Transform* transform) 
		: Collider(game_obj, transform) {
		from_trans = true;
		rotate_at_offset = false;
		calculateBoundingBox();
	}

	ColliderCircle::ColliderCircle(CollisionHandler* game_obj, Transform* transform, float x, float y, float rad)
		: Collider(game_obj, transform, x, y) {
		x_off = x;
		y_off = y;
		this->rad = rad;
		rotate_at_offset = true;
		from_trans = false;
		calculateBoundingBox();
	}

	void ColliderCircle::calculateBoundingBox() {
		float x0, x1, y0, y1, int_width, angle = (float)(M_PI * transform->rotz / 180.f);

		if (from_trans) {
			int_width = transform->w * (float)transform->scale_x;
			rad = (float)(int_width / 2);
			x = (float)transform->x + x_off + rad;
			y = (float)transform->y + y_off + rad;
		} else {
			x = (float)transform->x + x_off;
			y = (float)transform->y + y_off;
		}

		if (rotate_at_offset) {
			x0 = x - transform->centre.x;
			y0 = y - transform->centre.y;

			x1 = (float)(x0 * cos(angle) - y0 * sin(angle));
			y1 = (float)(x1 * sin(angle) + y0 * cos(angle));

			x = x1 + (float)transform->centre.x;
			y = y1 + (float)transform->centre.y;
		}

		min_and_max_x = glm::vec2{ x - rad, x + rad };
		min_and_max_y = glm::vec2{ y - rad, y + rad };
	}

	void ColliderCircle::recalculate() {
		calculateBoundingBox();
	}

	std::optional<glm::vec2> ColliderCircle::checkCollision(ColliderRect& other) {
		double tx = x, ty = y, dx, dy, dist, depth;
		glm::vec2 dir;

		if (x < other.left)
			tx = other.left;
		else if (x > other.right)
			tx = other.right;

		if (y < other.top)
			ty = other.top;
		else if (y > other.bottom)
			ty = other.bottom;

		dx = x - tx;
		dy = y - ty;

		dist = sqrt(pow(dx, 2) + pow(dy, 2));

		if (dist < rad) {
			depth = rad - dist;

			dir = dist == 0
				? dir = glm::normalize(transform->getLastDirection())
				: dir = glm::normalize(glm::vec2{ dx, dy }) * (float)depth;

			return std::make_optional<glm::vec2>(dir);
		}

		return std::nullopt;
	}

	void ColliderCircle::draw(SDL_Color color) {
		// wait for display
	}

	std::optional<glm::vec2> ColliderCircle::checkCollision(ColliderCircle& other) {
		double dist, depth, radsq, xpen, ypen;
		glm::vec2 dir;

		xpen = pow(other.x - x, 2);
		ypen = pow(other.y - y, 2);
		radsq = pow(other.rad + rad, 2);

		dist = xpen + ypen;
		depth = other.rad + rad - sqrt(dist);

		if (dist <= radsq) {
			auto vec = glm::normalize(glm::vec2{ x - other.x, y - other.y }) * (float)depth;
			return std::make_optional<glm::vec2>(vec);
		}
		
		return std::nullopt;
	}

	std::optional<glm::vec2> ColliderCircle::checkCollision(glm::vec2 point) {
		if (point.x >= min_and_max_x.x && // left
			point.x <= min_and_max_x.y && // right
			point.y >= min_and_max_y.x && // top
			point.y <= min_and_max_x.y) // bottom
			return std::optional<glm::vec2>(glm::vec2{ 0,0 });

		return std::nullopt;
	}
}
*/