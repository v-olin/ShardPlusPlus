#include "PhysicsBody.h"
#include "GameObject.h"
#include "ColliderRect.h"
#include "ColliderCircle.h"
#include "PhysicsManager.h"

#include <limits>

namespace Shard {
	PhysicsBody::PhysicsBody(GameObject* game_obj) {
		debug_color_ = SDL_Color{ 0, 255, 0 }; // green

		parent = game_obj;
		trans = game_obj->transform_;
		coll_handler = game_obj->body_->coll_handler;

		angular_drag = 0.01f;
		drag = 0.01f;
		mass = 1.f;
		max_force = 10.f;
		max_torque = 2.f;
		uses_gravity = false;
		stop_on_collision = true;
		reflect_on_collision = false;

		time_interval_ = PhysicsManager::getInstance().time_interval;
		PhysicsManager::getInstance().addPhysicsObject(*this);
	}

	void PhysicsBody::applyGravity(glm::vec2 dir, const float multiplier) {
		glm::vec2 gf = dir * multiplier;

		addForce(gf);
	}

	void PhysicsBody::draw() {
		for (auto collider : circ_colliders) {
			collider->draw(debug_color_);
		}
		for (auto collider : rect_colliders) {
			collider->draw(debug_color_);
		}
	}

	glm::vec2 PhysicsBody::getMinAndMax(const bool x) {
		float min = std::numeric_limits<float>::max();
		float max = std::numeric_limits<float>::min();
		glm::vec2 tmp(min, max);

		for (auto collider : circ_colliders) {
			tmp = x ? collider->min_and_max_x : collider->min_and_max_y;
			min = tmp.x < min ? tmp.x : min;
			max = tmp.y > max ? tmp.y : max;
		}

		for (auto collider : rect_colliders) {
			tmp = x ? collider->min_and_max_x : collider->min_and_max_y;
			min = tmp.x < min ? tmp.x : min;
			max = tmp.y > max ? tmp.y : max;
		}

		return glm::vec2{ min,max };
	}
	void PhysicsBody::addTorque(const float dir) {
		if (is_kinematic)
			return;

		torque_ += dir / mass;

		if (torque_ > max_torque)
			torque_ = max_torque;

		if (torque_ < max_torque * -1)
			torque_ = max_torque * -1;
	}

	void PhysicsBody::reverseForces(const float prop) {
		if (is_kinematic)
			return;

		force_ *= -prop;
	}

	void PhysicsBody::impartForces(PhysicsBody other, const float mass_prop) {
		other.addForce(force_ * mass_prop);

		recalculateColliders();
	}

	void PhysicsBody::stopForces() {
		force_ = glm::vec2(0, 0);
	}

	void PhysicsBody::reflectForces(glm::vec2 impulse) {
		glm::vec2 reflect(0,0);

		reflect.x = impulse.x > 0 ? 1 : -1;
		reflect.y = impulse.y > 0 ? 1 : -1;

		force_ *= reflect;
	}

	void PhysicsBody::reduceForces(const float prop) {
		force_ *= prop;
	}

	void PhysicsBody::addForce(glm::vec2 dir, const float force) {
		addForce(dir * force);
	}

	void PhysicsBody::addForce(glm::vec2 dir) {
		if (is_kinematic)
			return;

		dir /= mass;

		if (dir.length() * dir.length() < 0.0001)
			return;

		force_ += dir;

		if (force_.length() > max_force) {
			force_ = glm::normalize(force_) * max_force;
		}
	}

	void PhysicsBody::recalculateColliders() {
		for (auto collider : circ_colliders) {
			collider->recalculate();
		}
		for (auto collider : rect_colliders) {
			collider->recalculate();
		}

		min_and_max_x = getMinAndMax(true);
		min_and_max_y = getMinAndMax(false);
	}

	void PhysicsBody::physicsTick() {
		std::vector<glm::vec2> toRemove;
		float force_mag = force_.length();
		float rot = torque_;

		if (abs(torque_) < angular_drag) {
			torque_ = 0;
		}
		else {
			int signbit = (torque_ > 0) ? 1 : ((torque_ < 0) ? -1 : 0);
			torque_ -= signbit * angular_drag;
		}

		trans.rotate(rot);
		trans.translate(force_);

		if (force_mag < drag) {
			stopForces();
		}
		else if (force_mag > 0) {
			force_ = (force_ / force_mag) * (force_mag - drag);
		}
	}

	void PhysicsBody::addRectCollider() {
		// TODO: research emplace
		rect_colliders.emplace_back(coll_handler, &parent->transform_);
	}

	void PhysicsBody::addRectCollider(int x, int y, int w, int h) {
		// TODO: research emplace
		rect_colliders.emplace_back(coll_handler, &parent->transform_, x, y, w, h);
	}

	void PhysicsBody::addCircleCollider() {
		// TODO: research emplace
		circ_colliders.emplace_back(coll_handler, &parent->transform_);
	}

	void PhysicsBody::addCircleCollider(int x, int y, int rad) {
		// TODO: research emplace
		circ_colliders.emplace_back(coll_handler, &parent->transform_, x, y, rad);
	}

	std::optional<glm::vec2> PhysicsBody::checkCollisions(glm::vec2 other) {
		std::optional<glm::vec2> dir;

		for (ColliderCircle* c : circ_colliders) {
			dir = c->checkCollision(other);

			if (dir.has_value())
				return dir.value();
		}

		for (ColliderRect* c : rect_colliders) {
			dir = c->checkCollision(other);

			if (dir.has_value())
				return dir.value();
		}


		return std::nullopt;
	}
}