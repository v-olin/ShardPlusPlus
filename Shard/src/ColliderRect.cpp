#include "ColliderRect.h"
#include "ColliderCircle.h"

#include "Bootstrap.h"
#include "Display.h"

namespace Shard {

	ColliderRect::ColliderRect() : Collider(nullptr, nullptr) {
		for_minkowski = true;
	}

	ColliderRect::ColliderRect(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Transform> transform)
		: Collider(game_obj, transform) {
		from_trans = true;
		rotate_at_offset = false;
		calculateBoundingBox();
	}

	ColliderRect::ColliderRect(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Transform> transform, float x, float y, float w, float h)
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

		tx = getLeft() - other.getRight();
		ty = other.getTop() - getBottom();
		bx = getRight() - other.getLeft();
		by = other.getBottom() - getTop();

		w = width + other.width;
		h = height + other.height;
		mink.width = w;
		mink.height = h;

		mink.box_top_left = glm::vec2(tx, ty);
		mink.box_bottom_right = glm::vec2(bx, by);

		return mink;
	}

	glm::vec2 ColliderRect::calculatePenetration(glm::vec2 point) {
		float min = abs(getRight() - point.x);
		//TODO, depends on framerate...
		float cutoff = 2.f;

		// getLeft() edge
		if (abs(point.x - getLeft()) <= min)
			return glm::vec2{ abs((point.x - getLeft()) + cutoff), point.y };

		// getBottom()
		if (abs(getBottom() - point.y) <= min)
			return glm::vec2{ point.x, abs((getBottom() - point.y) + cutoff) };

		// getTop()
		if (abs(getTop() - point.y) <= min)
			return glm::vec2{ point.x, -1 * abs(getTop() - point.y) - cutoff };

		// getRight()
		return glm::vec2{ -1 * abs(getRight() - point.x) - cutoff, point.y };
	}

	//
	// inherited functions
	//
	void ColliderRect::recalculate() {
		calculateBoundingBox();
	}

	std::optional<glm::vec2> ColliderRect::checkCollision(std::shared_ptr<Collider> other) {
		auto circ_p = std::dynamic_pointer_cast<ColliderCircle>(other);
		auto rect_p = std::dynamic_pointer_cast<ColliderRect>(other);

		if (circ_p) {
			return checkCollision(circ_p);
		}
		else if (rect_p) {
			return checkCollision(rect_p);
		}
		else {
			return std::nullopt;
		}
	}

	std::optional<glm::vec2> ColliderRect::checkCollision(std::shared_ptr<ColliderRect> other) {
		ColliderRect cr = calculateMinkowskiDifference(*other);

		if (cr.getLeft() <= 0 && cr.getRight() >= 0 && cr.getTop() <= 0 && cr.getBottom() >= 0) {
			glm::vec2 pen = cr.calculatePenetration(glm::vec2{ 0,0 });
			return std::make_optional<glm::vec2>(pen);
		}

		return std::nullopt;
	}

	std::optional<glm::vec2> ColliderRect::checkCollision(std::shared_ptr<ColliderCircle> circ) {
		////////////////////////////////////////auto this_ = std::make_shared<ColliderRect>(this);
		std::optional<glm::vec2> possible_v = circ->checkCollision(shared_from_this());
		
		if (possible_v.has_value()) {
			possible_v.value().x *= -1;
			possible_v.value().y *= -1;
			return possible_v;
		}

		return std::nullopt;
	}

	std::optional<glm::vec2> ColliderRect::checkCollision(glm::vec2 other) {
		if (other.x >= getLeft() &&
			other.x <= getRight() &&
			other.y >= getTop() &&
			other.y <= getBottom())
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

		// getTop()
		d->drawLine(tx, ty, bx, ty, color);

		// getRight()
		d->drawLine(bx, ty, bx, by, color);

		// getBottom()
		d->drawLine(tx, by, bx, by, color);

		// getLeft()
		d->drawLine(tx, ty, tx, by, color);

	}

}
