#include "PhysicsBody.h"
#include "PhysicsManager.h"

#include "ColliderBox.h"

#include <limits>
#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/norm.hpp>

#include "GameObject.h"

namespace Shard {
	PhysicsBody::PhysicsBody()
		: trans(std::make_shared<Transform>())
	{
	}

	PhysicsBody::PhysicsBody(std::shared_ptr<GameObject> game_obj)
		: parent(game_obj)
		, trans(std::make_shared<Transform>())
		, debug_color({ 0.0f, 1.0f, 0.0f })
	{
		time_interval_ = PhysicsManager::getInstance().time_interval;
	}

	void PhysicsBody::applyGravity(glm::vec3 dir, const float multiplier) {
		addForce(dir * multiplier);
	}

	void PhysicsBody::draw() {
		collider->draw(debug_color);
	}

	void PhysicsBody::addTorque(glm::vec3 how_much) {
		if (is_kinematic)
			return;

		torque += how_much;

		// Cap to maximum torque if needed
		//torque = std::min(torque, max_torque);
		//torque = std::max(torque, -max_torque);
		
		torque.x = std::min(torque.x, max_torque.x);
		torque.x = std::min(torque.y, max_torque.y);
		torque.z = std::min(torque.z, max_torque.z);

		// Cap to minimum torque if needed
		torque.x = std::max(-max_torque.x, torque.x);
		torque.y = std::max(-max_torque.y, torque.y);
		torque.z = std::max(-max_torque.z, torque.z);

	}

	void PhysicsBody::reverseForces(const float prop) {
		if (is_kinematic)
			return;

		force *= -prop;
	}

	void PhysicsBody::impartForces(std::shared_ptr<PhysicsBody> other, const float mass_prop) {
		other->addForce(this->force * mass_prop);

		recalculateColliders();
	}

	void PhysicsBody::stopForces() {
		force = glm::vec3(0, 0, 0);
	}

	void PhysicsBody::reflectForces(glm::vec3 impulse) {
		glm::vec3 reflect(0,0, 0);

		reflect.x = impulse.x > 0 ? 1.f : -1.f;
		reflect.y = impulse.y > 0 ? 1.f : -1.f;
		reflect.z = impulse.z > 0 ? 1.f : -1.f;

		force *= reflect;
	}

	void PhysicsBody::reduceForces(const float prop) {
		force *= prop;
	}

	void PhysicsBody::addForce(glm::vec3 dir, const float force) {
		addForce(dir * force);
	}

	void PhysicsBody::addForce(glm::vec3 incoming_force) {
		if (is_kinematic)
			return;

		// TODO: double check if correct?
		incoming_force /= mass;
		if (glm::length2(incoming_force) < 0.0001)
			return;

		force += incoming_force;

		// Cap the force
		force.x = std::min(force.x, max_force.x);
		force.y = std::min(force.y, max_force.y);
		force.z = std::min(force.z, max_force.z);

	}

	void PhysicsBody::recalculateColliders() {
		collider->recalculateBoundingBox();
	}

	void PhysicsBody::physicsTick() {
		std::vector<glm::vec2> toRemove;
		float rotx = torque.x;
		float roty = torque.y;
		float rotz = torque.z;

		static auto get_sign_bit = [](float n) {
			return n == 0 ? 0 : (n > 0 ? 1 : -1);
		};

		static auto apply_drag = [&](float drag, int idx) {
			float val = torque[idx];
			torque[idx] -= get_sign_bit(val) * drag;
		};

		apply_drag(angular_drag.x, 0);
		apply_drag(angular_drag.y, 1);
		apply_drag(angular_drag.z, 2);

		trans->rotate(torque.x, glm::vec3{1.0, 0.0, 0.0});
		trans->rotate(torque.y, glm::vec3{0.0, 1.0, 0.0});
		trans->rotate(torque.z, glm::vec3{0.0, 0.0, 1.0});

		float force_mag = glm::length(force);
		trans->translate(force);

		if (force_mag < drag) {
			stopForces();
		}
		else if (force_mag > 0) {
			// TODO: Make sure below is correct from force = (force / force_mag) * (force_mag - drag); to vvvvvvvv
			force = glm::normalize(force) * (force_mag - drag);
		}
	}

	void PhysicsBody::setBoxCollider() {
		auto handler = std::dynamic_pointer_cast<CollisionHandler>(parent);
		collider = std::make_shared<ColliderBox>(handler, trans->shared_from_this());
	}

	//void PhysicsBody::setBoxCollider(float x, float y, float z, float w, float h, float d) {
	//	auto handler = std::dynamic_pointer_cast<CollisionHandler>(parent);
	//	collider = std::make_shared<ColliderBox>(handler, trans->shared_from_this(), x, y, z, w, h, d);
	//}

	std::optional<glm::vec2> PhysicsBody::checkCollision(Ray& ray) {
		std::optional<glm::vec2> dir{ collider->checkCollision(ray) };
		if (dir.has_value())
			return dir.value();

		return std::nullopt;
	}
}