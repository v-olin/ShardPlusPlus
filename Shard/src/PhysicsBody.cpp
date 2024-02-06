#include "PhysicsBody.h"
#include "PhysicsManager.h"

#include "ColliderCircle.h"
#include "ColliderRect.h"

#include <limits>
#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/norm.hpp>

#include "GameObject.h"

namespace Shard {
	PhysicsBody::PhysicsBody()
		: 
		force_(0,0),
		time_interval_(0),
		torque_(0),
		angular_drag(0),
		drag(0),
		mass(1),
		min_and_max_x(0, 0),
		min_and_max_y(0, 0),
		max_force(50),
		max_torque(100),
		is_kinematic(false),
		pass_through(false),
		uses_gravity(false),
		stop_on_collision(true),
		reflect_on_collision(false),
		impart_force(false),
		parent(nullptr)
	{
		trans = std::make_shared<Transform3D>();
	}

	PhysicsBody::PhysicsBody(std::shared_ptr<GameObject> game_obj)
		:
		angular_drag(0.01f),
		drag(0.01f),
		mass(1),
		min_and_max_x(0, 0),
		min_and_max_y(0, 0),
		max_force(50),
		max_torque(2.f),
		is_kinematic(false),
		pass_through(false),
		uses_gravity(false),
		stop_on_collision(true),
		reflect_on_collision(false),
		impart_force(false)
	{
		debug_color_ = SDL_Color{ 0, 255, 0, 255 }; // green
		parent = game_obj;

		trans = std::make_shared<Transform3D>();

		time_interval_ = PhysicsManager::getInstance().time_interval;
		//PhysicsManager::getInstance().addPhysicsObject(shared_from_this());
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

	// const bool :nerd_face_emoji:
	glm::vec2 PhysicsBody::getMinAndMax(const bool x) {
		int min = std::numeric_limits<int>::max();
		int max = std::numeric_limits<int>::min();

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

	void PhysicsBody::impartForces(std::shared_ptr<PhysicsBody> other, const float mass_prop) {
		other->addForce(force_ * mass_prop);

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
		//if (force.length() * force.length() < 0.0001)
		//if (glm::dot(force, force) < 0.0001)
		if (glm::length2(force) < 0.0001)
			return;

		force_ += force;

		// Cap the force
		//if (force_.length() > max_force)
		if (glm::length(force_) > max_force)
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
			else 
				sign_bit = 0;
			torque_ -= sign_bit * angular_drag;

		}

		trans->rotate(rot);

		float force_mag = glm::length(force_);
		trans->translate(force_);

		if (force_mag < drag) {
			stopForces();
		}
		else if (force_mag > 0) {
			force_ = (force_ / force_mag) * (force_mag - drag);
		}
	}

	// TODO: All of these leak memory :)
	// not anymore!!! (hopefully)
	void PhysicsBody::addRectCollider() {
		auto handler = std::dynamic_pointer_cast<CollisionHandler>(parent);
		auto col = std::make_shared<ColliderRect>(handler, trans->shared_from_this());
		colliders.push_back(col);
	}

	void PhysicsBody::addRectCollider(float x, float y, float w, float h) {
		auto handler = std::dynamic_pointer_cast<CollisionHandler>(parent);
		auto col = std::make_shared<ColliderRect>(handler, trans->shared_from_this(), x, y, w, h);
		colliders.push_back(col);
	}

	void PhysicsBody::addCircleCollider() {
		auto handler = std::dynamic_pointer_cast<CollisionHandler>(parent);
		auto col = std::make_shared<ColliderCircle>(handler, trans->shared_from_this());
		colliders.push_back(col);
	}

	void PhysicsBody::addCircleCollider(float x, float y, float rad) {
		auto handler = std::dynamic_pointer_cast<CollisionHandler>(parent);
		auto col = std::make_shared<ColliderCircle>(handler, trans->shared_from_this(), x, y, rad);
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