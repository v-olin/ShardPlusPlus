#include "PhysicsBody.h"

#include <limits>

namespace Shard {
	PhysicsBody::PhysicsBody() { // PhysicsBody(GameObject obj)
		debug_color_ = SDL_Color{ 0, 255, 0 }; // green

		// Parent = obj;
		// Transform = obj.transform;
		// coll_handler = (CollisionHandler)p;

		angular_drag = 0.01f;
		drag = 0.01f;
		mass = 1.f;
		max_force = 10.f;
		max_torque = 2.f;
		uses_gravity = false;
		stop_on_collision = true;
		reflect_on_collision = false;

		// time_interval_ = PhysicsManager.getInstance().TimeInterval;
		// PhysicsManager.getInstance().addPhysicsObject(this);
	}

	void PhysicsBody::applyGravity(glm::vec2 dir, const float multiplier) {
		glm::vec2 gf = dir * multiplier;

		addForce(gf);
	}

	// TODO: fix this
	void PhysicsBody::draw() {
		// foreach (collider in myColliders
		//		collider.draw(debug_color_);
	}

	glm::vec2 PhysicsBody::getMinAndMax(const bool x) {
		float min = std::numeric_limits<float>::max();
		float max = std::numeric_limits<float>::min();
		glm::vec2 tmp(min, max);

		/*
		foreach (collider in myColliders) {
			if (x)
				tmp.x = col.minandmaxX
			else
				tmp.y = col.minandmaxY

			if (tmp[0] < min)
				min = tmp[0]
			if (tmp[1] > max)
				max = tmp[1]
		}

		return Vec2(min, max);

		*/

		return tmp;
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
		//foreach(Collider col in getColliders())
		//{
		//	col.recalculate();
		//}
		//
		//MinAndMaxX = getMinAndMax(true);
		//MinAndMaxY = getMinAndMax(false);
	}

	void PhysicsBody::physicsTick() {
		std::vector<glm::vec2> toRemove;
		float force_mag = force_.length();
		float rot = torque_;

		//Math.Abs(torque) < AngularDrag
		if (abs(torque_) < angular_drag) {
			torque_ = 0;
		}
		else {
			int signbit = (torque_ > 0) ? 1 : ((torque_ < 0) ? -1 : 0);
			torque_ -= signbit * angular_drag;
		}

		//trans.rotate(rot);
		//trans.translate(this.force);

		if (force_mag < drag) {
			stopForces();
		}
		else if (force_mag > 0) {
			force_ = (force_ / force_mag) * (force_mag - drag);
		}
	}
}