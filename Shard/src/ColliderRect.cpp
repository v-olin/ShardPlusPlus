/*
#include <math.h>
#include <numeric>

#include "Collider.h"
#include "ColliderRect.h"
#include "ColliderCircle.h"
#include "Transform.h"

namespace Shard {
	ColliderRect::ColliderRect() : Collider(nullptr, nullptr) {
		for_minkowski = true;
	}

	ColliderRect::ColliderRect(CollisionHandler* game_obj, Transform* transform) : Collider(game_obj, transform) {
		from_trans = true;
		rotate_at_offset = false;
		calculateBoundingBox();
	}

	ColliderRect::ColliderRect(CollisionHandler* game_obj, Transform* transform, float x, float y, float w, float h)
		: Collider(game_obj, transform, x, y) {
		this->width = w;
		this->height = h;

		rotate_at_offset = true;
		from_trans = false;
	}

	void ColliderRect::calculateBoundingBox() {
		float n_width, n_height, angle, x0, x1, y0, y1;
		double cos_, sin_;

		// this will never happen, surely
		//if (rect == NULL)
		//	return;

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
			y1 = (float)(x0 * sin(angle) + y0 * sin(angle));

			x = x1 + (float)transform->centre.x;
			y = y1 + (float)transform->centre.y;
		}

		left = x - width / 2;
		right = x + width / 2;
		top = y - height / 2;
		bottom = y + height / 2;
	}

	ColliderRect ColliderRect::calculateMinkowskiDifference(ColliderRect& other) {
		float l, r, t, b, w, h;
		ColliderRect mink = ColliderRect();

		l = left - other.right;
		t = other.top - bottom;
		w = width + other.width;
		h = height + other.height;
		b = other.bottom - top;

		mink.width = w;
		mink.height = h;

		mink.min_and_max_x = glm::vec2{ left, right };
		mink.min_and_max_y = glm::vec2{ top, bottom };

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

	std::optional<glm::vec2> ColliderRect::checkCollision(ColliderRect& other) {
		ColliderRect cr = calculateMinkowskiDifference(other);

		if (cr.left <= 0 && cr.right >= 0 && cr.top <= 0 && cr.bottom >= 0) {
			glm::vec2 pen = cr.calculatePenetration(glm::vec2{ 0,0 });
			std::make_optional<glm::vec2>(pen);
		}

		return std::nullopt;
	}

	void ColliderRect::draw(SDL_Color color) {
		// cannot do until display is finished
	}

	std::optional<glm::vec2> ColliderRect::checkCollision(ColliderCircle& c) {
		glm::vec2 possible_v = c.checkCollision(*(this));
		glm::vec2 null_v = NULL_VECTOR;

		// if not nullvector
		if (possible_v.x != null_v.x || possible_v.y != null_v.y) {
			possible_v.x *= -1;
			possible_v.y *= -1;
			return std::make_optional<glm::vec2>(possible_v);
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
}
*/