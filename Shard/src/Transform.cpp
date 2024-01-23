#include "Transform.h"
#include "VectorUtility.h"

#include <math.h>

namespace Shard {
	Transform::Transform() { }

	Transform::Transform(GameObject* owner) {
		this->owner = owner;
		forward = NULL_VECTOR;
		right = NULL_VECTOR;
		centre = NULL_VECTOR;

		scale_x = 1.f;
		scale_y = 1.f;

		x = 0; y = 0;
		lx = 0; ly = 0;

		rotate(0);
	}

	void Transform::recalculateCentre() {
		centre.x = (float)(x + (w * scale_x / 2));
		centre.y = (float)(y + (h * scale_y / 2));
	}

	void Transform::translate(float nx, float ny) {
		lx = x;
		ly = y;

		x += nx;
		y += ny;

		recalculateCentre();
	}

	void Transform::translate(glm::vec2 v) {
		translate(v.x, v.y);
	}

	void Transform::rotate(float dir) {
		rotz += dir;
		rotz = fmod(rotz, 360);

		float angle = (M_PI * rotz / 180.f);
		float sin_ = sin(angle);
		float cos_ = cos(angle);

		forward.x = cos_;
		forward.y = sin_;

		right.x = -1 * forward.y;
		right.y = forward.x;
	}

	glm::vec2 Transform::getLastDirection() {
		float dx = x - lx, dy = y - ly;
		return glm::vec2{ -dx, -dy };
	}
}