#include "ColliderRect.h"
#include "ColliderCircle.h"

#include "Bootstrap.h"
#include "Display.h"

namespace Shard {

	ColliderRect::ColliderRect() : Collider(nullptr, nullptr) {
		for_minkowski = true;
	}

	ColliderRect::ColliderRect(CollisionHandler* game_obj, Transform* transform)
		: Collider(game_obj, transform) {
		from_trans = true;
		rotate_at_offset = false;
		calculateBoundingBox();
	}

	ColliderRect::ColliderRect(CollisionHandler* game_obj, Transform* transform, float x, float y, float w, float h)
		: Collider(game_obj, transform, x, y) {
		this->base_width = w;
		this->base_height = h;

		rotate_at_offset = true;
		from_trans = false;
	}

	void ColliderRect::calculateBoundingBox() {
		float n_width, n_height, angle, x0, x1, y0, y1;
		double cos_, sin_;

		if (from_trans) {
			width = (float)(transform->w * transform->scale_x);
			height = (float)(transform->h * transform->scale_y);
		}
		else {
			width = (float)(base_width * transform->scale_x);
			height = (float)(base_height * transform->scale_y);
		}

		angle = (float)(M_PI * transform->rotz / 180.f);

		cos_ = cos(angle);
		sin_ = sin(angle);

		n_width = (float)(abs(width * cos_) + abs(height * sin_));
		n_height = (float)(abs(width * sin_) + abs(height * cos_));

		x = (float)transform->x + (width / 2);
		y = (float)transform->y + (height / 2);

		width = n_width;
		height = n_height;

		if (rotate_at_offset) {
			x0 = x - transform->centre.x;
			y0 = y - transform->centre.y;

			x1 = (float)(x0 * cos(angle) - y0 * sin(angle));
			y1 = (float)(x0 * sin(angle) + y0 * cos(angle));

			x = x1 + (float)transform->centre.x;
			y = y1 + (float)transform->centre.y;
		}

		int tx = x - width / 2.0f;
		int ty = y - height / 2.0f;

		int bx = x + width / 2.0f;
		int by = y + height / 2.0f;

		box_top_left = glm::vec2(tx, ty);
		box_bottom_right = glm::vec2(bx, by);

	}

	ColliderRect ColliderRect::calculateMinkowskiDifference(ColliderRect& other) {
		float tx, ty, bx, by, w, h;
		ColliderRect mink = ColliderRect();

		tx = left - other.right;
		ty = other.top - bottom;
		bx = right - other.left;
		by = other.bottom - top;

		w = width + other.width;
		h = height + other.height;
		mink.width = w;
		mink.height = h;

		mink.box_top_left = glm::vec2(tx, ty);
		mink.box_bottom_right = glm::vec2(bx, by);

		return mink;
	}

	glm::vec2 ColliderRect::calculatePenetration(glm::vec2 point) {
		float min = abs(right - point.x);
		float cutoff = 0.2f;

		// left edge
		if (abs(point.x - left) <= min)
			return glm::vec2{ abs(point.x - left + cutoff), point.y };

		// bottom
		if (abs(bottom - point.y) <= min)
			return glm::vec2{ point.x, abs(bottom - point.y + cutoff) };

		// top
		if (abs(top - point.y) <= min)
			return glm::vec2{ point.x, -1 * abs(top - point.y) - cutoff };

		// right
		return glm::vec2{ -1 * abs(right - point.x) - cutoff, point.y };
	}

	//
	// inherited functions
	//
	void ColliderRect::recalculate() {
		calculateBoundingBox();
	}

	std::optional<glm::vec2> ColliderRect::checkCollision(Collider* other) {
		return other->checkCollision(this);
	}

	std::optional<glm::vec2> ColliderRect::checkCollision(ColliderRect* other) {
		ColliderRect cr = calculateMinkowskiDifference(*other);

		if (cr.left <= 0 && cr.right >= 0 && cr.top <= 0 && cr.bottom >= 0) {
			glm::vec2 pen = cr.calculatePenetration(glm::vec2{ 0,0 });
			return std::make_optional<glm::vec2>(pen);
		}

		return std::nullopt;
	}

	std::optional<glm::vec2> ColliderRect::checkCollision(ColliderCircle* circ) {
		std::optional<glm::vec2> possible_v = circ->checkCollision(this);

		if (possible_v.has_value()) {
			possible_v.value().x *= -1;
			possible_v.value().y *= -1;
			return possible_v;
		}

		return std::nullopt;
	}

	std::optional<glm::vec2> ColliderRect::checkCollision(glm::vec2 other) {
		if (other.x >= left &&
			other.x <= right &&
			other.y >= top &&
			other.y <= bottom)
			return std::make_optional<glm::vec2>(glm::vec2{ 0,0 });

		return std::nullopt;
	}

	void ColliderRect::draw(SDL_Color color) {
		// TODO: cannot do until display is finished
		Display* d = Bootstrap::getDisplay();
		recalculate();

		int tx = box_top_left.x;
		int ty = box_top_left.y;

		int bx = box_bottom_right.x;
		int by = box_bottom_right.y;

		// Top
		d->drawLine(tx, ty, bx, ty, color);

		// Right
		d->drawLine(bx, ty, bx, by, color);

		// Bottom
		d->drawLine(tx, by, bx, by, color);

		// Left
		d->drawLine(tx, ty, tx, by, color);

	}

}
