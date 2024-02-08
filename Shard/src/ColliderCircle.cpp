#include "ColliderCircle.h"
#include "ColliderRect.h"

#include "Bootstrap.h"
#include "Display.h"

namespace Shard {

	ColliderCircle::ColliderCircle() :
		rad(0), x_off(0), y_off(0) {
	}

	ColliderCircle::ColliderCircle(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Transform> transform)
		: Collider(game_obj, transform) {
		from_trans = true;
		rotate_at_offset = false;
		calculateBoundingBox();
	}

	ColliderCircle::ColliderCircle(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Transform> transform, float x, float y, float rad)
		: Collider(game_obj, transform, x, y) {
		x_off = x;
		y_off = y;
		this->rad = rad;
		rotate_at_offset = true;
		from_trans = false;
		calculateBoundingBox();
	}

	void ColliderCircle::calculateBoundingBox() {
		float x0, x1, y0, y1, int_width;
		float angle = (float)(M_PI * transform->rotz / 180.f);

		if (from_trans) {
			int_width = transform->w * (float)transform->scale_x;
			rad = (float)(int_width / 2);

			x = (float)transform->x + x_off + rad;
			y = (float)transform->y + y_off + rad;
		}
		else {
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

		box_top_left.x = x - rad;
		box_top_left.y = y - rad;

		box_bottom_right.x = x + rad;
		box_bottom_right.y = y + rad;

	}

	void ColliderCircle::recalculate() {
		calculateBoundingBox();
	}

	std::optional<glm::vec2> ColliderCircle::checkCollision(std::shared_ptr<ColliderRect> other) {
		double tx = x;
		double ty = y;
		int dx, dy, dist, depth;
		glm::vec2 dir;

		if (x < other->getLeft())
			tx = other->getLeft();
		else if (x > other->getRight())
			tx = other->getRight();

		if (y < other->getTop())
			ty = other->getTop();
		else if (y > other->getBottom())
			ty = other->getBottom();

		dx = x - tx;
		dy = y - ty;

		//dist = sqrt(pow(dx, 2) + pow(dy, 2));
		dist = glm::length(glm::vec2(dx, dy));

		if (dist < rad) {
			depth = rad - dist;

			dir = dist == 0
				? glm::normalize(transform->getLastDirection())
				: glm::normalize(glm::vec2{ dx, dy }) * (float)depth;

			return std::make_optional<glm::vec2>(dir);
		}

		return std::nullopt;
	}

	void ColliderCircle::draw(SDL_Color color) {
		Display* d = Bootstrap::getDisplay();
		d->drawCircle((int)x, (int)y, rad, color);
	}

	std::optional<glm::vec2> ColliderCircle::checkCollision(std::shared_ptr<ColliderCircle> other) {
		double dist, depth, radsq, xpen, ypen;

		xpen = pow(other->x - x, 2);
		ypen = pow(other->y - y, 2);
		radsq = pow(other->rad + rad, 2);

		dist = xpen + ypen;
		depth = other->rad + rad - sqrt(dist);

		if (dist <= radsq) {
			auto vec = glm::normalize(glm::vec2{ x - other->x, y - other->y }) * (float)depth;
			return std::make_optional<glm::vec2>(vec);
		}

		return std::nullopt;
	}

	std::optional<glm::vec2> ColliderCircle::checkCollision(glm::vec2 point) {
		if (point.x >= box_top_left.x &&
			point.x <= box_bottom_right.x &&
			point.y >= box_top_left.y &&
			point.y <= box_bottom_right.y) {
			return std::optional<glm::vec2>({ 0, 0 });
		}

		return std::nullopt;
	}

	std::optional<glm::vec2> ColliderCircle::checkCollision(std::shared_ptr<Collider> other) {
		//auto this_ = std::make_shared<ColliderCircle>(this);
		return other->checkCollision(shared_from_this());
	}

}