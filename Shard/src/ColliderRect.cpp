#include <math.h>
#include <numeric>

#include "Collider.h"
#include "ColliderRect.h"

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
			width = (float)(rect.w * rect.scale_x);
			height = (float)(rect.h * rect.scale_y);
		}
		else {
			width = (float)(base_width * rect.scale_x);
			height = (float)(base_height * rect.scale_y);
		}

		angle = (float)(M_PI * rect.rotz / 180.f);

		cos_ = cos(angle);
		sin_ = sin(angle);

		n_width = (float)(abs(width * cos_) + abs(height * sin_));
		n_height = (float)(abs(width * sin_) + abs(height * cos_));

		x = (float)rect.x + (width / 2);
		y = (float)rect.y + (height / 2);

		width = n_width;
		height = n_height;

		if (rotate_at_offset) {
			x0 = x - rect.centre.x;
			y0 = y - rect.centre.y;

			x1 = (float)(x0 * cos(angle) - y0 * sin(angle));
			y1 = (float)(x0 * sin(angle) + y0 * sin(angle));

			x = x1 + (float)rect.centre.x;
			y = y1 + (float)rect.centre.y;
		}

		left = x - width / 2;
		right = x + width / 2;
		top = y - height / 2;
		bottom = y + height / 2;
	}

	ColliderRect ColliderRect::calculateMinkowskiDifference(ColliderRect& other) {
		float l, r, t, b, w, h;
		ColliderRect mink = ColliderRect();
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

	glm::vec2 ColliderRect::checkCollision(ColliderRect& other) {
		ColliderRect cr = calculateMinkowskiDifference(other);

		if (cr.left <= 0 && cr.right >= 0 && cr.top <= 0 && cr.bottom >= 0) {
			return cr.calculatePenetration(glm::vec2{ 0,0 });
		}

		// null vector kinda
		return NULL_VECTOR;
	}

	void ColliderRect::draw(SDL_Color color) {
		// cannot do until display is finished
	}

	glm::vec2 ColliderRect::checkCollision(ColliderCircle& c) {
		// stinky ugly hack
		ColliderRect& this_ = *(this);

		glm::vec2 possible_v = c.checkCollision(this_);
		glm::vec2 null_v = NULL_VECTOR;
		
		// if not nullvector
		if (possible_v.x != null_v.x || possible_v.y != null_v.y) {
			possible_v.x *= -1;
			possible_v.y *= -1;
			return possible_v;
		}
	
		return NULL_VECTOR;
	}

	glm::vec2 ColliderRect::checkCollision(glm::vec2 other) {
		if (other.x >= left &&
			other.x <= right &&
			other.y >= top &&
			other.y <= bottom)
			return glm::vec2{ 0,0 };

		return NULL_VECTOR;
	}
}