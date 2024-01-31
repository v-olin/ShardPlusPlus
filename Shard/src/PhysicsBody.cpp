#include "PhysicsBody.h"
#include "PhysicsManager.h"

#include <limits>
#include <algorithm>

namespace Shard {
	PhysicsBody::PhysicsBody()
		: 
		angular_drag(0),
		drag(0),
		mass(1),
		min_and_max_x(0, 0),
		min_and_max_y(0, 0),
		max_force(50),
		max_torque(100),
		is_kinematic(true),
		allows_pass_through(false),
		uses_gravity(true),
		stop_on_collision(false),
		reflect_on_collision(true),
		impart_force(true),
		parent(nullptr),
		coll_handler(nullptr)
	{
	}

	PhysicsBody::PhysicsBody(GameObject* game_obj) {
		debug_color_ = SDL_Color{ 0, 255, 0, 255 }; // green
		parent = game_obj;

		// TODO: initializer list
		angular_drag = 0.01f;
		drag = 0.01f;
		mass = 1.f;
		max_force = 10.f;
		max_torque = 2.f;
		uses_gravity = false;
		stop_on_collision = true;
		reflect_on_collision = false;

		time_interval_ = PhysicsManager::getInstance().time_interval;
		PhysicsManager::getInstance().addPhysicsObject(this);
	}

	void PhysicsBody::applyGravity(glm::vec2 dir, const float multiplier) {
		glm::vec2 gf = dir * multiplier;

		addForce(gf);
	}

	void PhysicsBody::draw() {
		for (auto &collider : colliders) {
			collider->draw(debug_color_);
		}
	}

	glm::vec2 PhysicsBody::getMinAndMax(const bool x) {
		int min = std::numeric_limits<int>::min();
		int max = std::numeric_limits<int>::max();

		for (auto &collider : colliders) {
			if (x) {
				// gets min and max x-values
				int tx = collider->box_top_left.x;
				int ex = collider->box_bottom_right.x;
				min = std::min(tx, min);
				max = std::max(ex, max);
			}
			else {
				// gets min and max y-values
				int ty = collider->box_top_left.y;
				int ey = collider->box_bottom_right.y;
				min = std::min(ty, min);
				max = std::max(ey, max);
			}
		}

		return { min, max };
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

		reflect.x = impulse.x > 0 ? 1.f : -1.f;
		reflect.y = impulse.y > 0 ? 1.f : -1.f;

		force_ *= reflect;
	}

	void PhysicsBody::reduceForces(const float prop) {
		force_ *= prop;
	}

	void PhysicsBody::addForce(glm::vec2 dir, const float force) {
		addForce(dir * force);
	}

	void PhysicsBody::addForce(glm::vec2 force) {
		if (is_kinematic)
			return;

		force /= mass;
		if (force.length() * force.length() < 0.0001)
			return;

		force_ += force;

		// Cap the force
		if (force_.length() > max_force)
			force_ = glm::normalize(force_) * max_force;
	}

	void PhysicsBody::recalculateColliders() {
		for (auto &collider : colliders)
			collider->recalculate();

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
			int sign_bit{};
			if (torque_ > 0)
				sign_bit = 1;
			else if (torque_ < 0)
				sign_bit = -1;
			else {
				sign_bit = 0;
			}
			torque_ -= sign_bit * angular_drag;

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
		Collider* col = new ColliderRect(coll_handler, &parent->body_->trans);
		colliders.push_back(col);
	}

	void PhysicsBody::addRectCollider(float x, float y, float w, float h) {
		Collider* col = new ColliderRect(coll_handler, &parent->body_->trans, x, y, w, h);
		colliders.push_back(col);
	}

	void PhysicsBody::addCircleCollider() {
		Collider* col = new ColliderCircle(coll_handler, &parent->body_->trans);
		colliders.push_back(col);
	}

	void PhysicsBody::addCircleCollider(float x, float y, float rad) {
		Collider* col = new ColliderCircle(coll_handler, &parent->body_->trans, x, y, rad);
		colliders.push_back(col);
	}

	std::optional<glm::vec2> PhysicsBody::checkCollisions(glm::vec2 other) {
		std::optional<glm::vec2> dir;

		for (auto& collider : colliders) {
			dir = collider->checkCollision(other);
			if (dir.has_value())
				return dir.value();
		}

		return std::nullopt;
	}
}