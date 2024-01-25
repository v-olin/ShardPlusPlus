#include "Transform.h"
#include "Collider.h"

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

		min_and_max_x = glm::vec2{ x - rad, x + rad };
		min_and_max_y = glm::vec2{ y - rad, y + rad };
	}

	void ColliderCircle::recalculate() {
		calculateBoundingBox();
	}

	std::optional<glm::vec2> ColliderCircle::checkCollision(ColliderRect* other) {
		double tx = x, ty = y, dx, dy, dist, depth;
		glm::vec2 dir;

		if (x < other->left)
			tx = other->left;
		else if (x > other->right)
			tx = other->right;

		if (y < other->top)
			ty = other->top;
		else if (y > other->bottom)
			ty = other->bottom;

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

	std::optional<glm::vec2> ColliderCircle::checkCollision(ColliderCircle* other) {
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
		if (point.x >= min_and_max_x.x && // left
			point.x <= min_and_max_x.y && // right
			point.y >= min_and_max_y.x && // top
			point.y <= min_and_max_x.y) // bottom
			return std::optional<glm::vec2>(glm::vec2{ 0,0 });

		return std::nullopt;
	}

	std::optional<glm::vec2> ColliderCircle::checkCollision(Collider* other) {
		ColliderCircle circ;
		ColliderRect rect;

		if (typeid(circ) == typeid(other)) {
			ColliderCircle* circ_p = dynamic_cast<ColliderCircle*>(other);
			return checkCollision(circ_p);
		}
		else if (typeid(rect) == typeid(other)) {
			ColliderRect* rect_p = dynamic_cast<ColliderRect*>(other);
			return checkCollision(rect_p);
		}
		else
			return std::nullopt;
	}

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
		r = right - other.left;
		b = other.bottom - top;

		w = width + other.width;
		h = height + other.height;
		mink.width = w;
		mink.height = h;

		mink.min_and_max_x = glm::vec2{ l, r };
		mink.min_and_max_y = glm::vec2{ t, b };

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
		ColliderCircle circ;
		ColliderRect rect;
		if (typeid(circ) == typeid(other)) {
			ColliderCircle* other_circ = dynamic_cast<ColliderCircle*>(other);
			return checkCollision(other_circ);
		}
		else if (typeid(rect) == typeid(other)) {
			ColliderRect* other_rect = dynamic_cast<ColliderRect*>(other);
			return checkCollision(other_rect);
		}
		else {
			// TODO: debug.log
			//oops, this is not good
			return std::nullopt;
		}
	}

	std::optional<glm::vec2> ColliderRect::checkCollision(ColliderRect* other) {
		ColliderRect cr = calculateMinkowskiDifference(*other);

		if (cr.left <= 0 && cr.right >= 0 && cr.top <= 0 && cr.bottom >= 0) {
			glm::vec2 pen = cr.calculatePenetration(glm::vec2{ 0,0 });
			return std::make_optional<glm::vec2>(pen);
		}

		return std::nullopt;
	}

	std::optional<glm::vec2> ColliderRect::checkCollision(ColliderCircle* c) {
		std::optional<glm::vec2> possible_v = c->checkCollision(this);

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
	}
}