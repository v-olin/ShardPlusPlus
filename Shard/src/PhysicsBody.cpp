#include "PhysicsBody.h"
#include "PhysicsManager.h"

#include "ColliderBox.h"
#include "ColliderBox.h"

#include <limits>
#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/norm.hpp>

#include "GameObject.h"

#define FLOAT_MAX std::numeric_limits<float>::max()
#define FLOAT_MIN std::numeric_limits<float>::min()

namespace Shard {
	PhysicsBody::PhysicsBody()
		: m_drag(0.f)
		, m_mass(0.f)
		, m_angularDrag(0.f)
		, m_maxForce({ FLOAT_MAX, FLOAT_MAX, FLOAT_MAX })
		, m_maxTorque({ FLOAT_MIN, FLOAT_MIN, FLOAT_MIN })
		, m_isKinematic(false)
		, m_passThrough(false)
		, m_usesGravity(false)
		, m_stopOnCollision(true)
		, m_reflectOnCollision(false)
		, m_impartForce(false)
		, m_clickable(true)
		, m_parent(nullptr)
		, m_collider(nullptr)
		, m_bodyModel(nullptr)
		, m_debugColor(1.f)
		, m_force(0.f)
		, m_torque(0.f)
		, m_timeInterval(0)
	{ }

	PhysicsBody::PhysicsBody(std::shared_ptr<GameObject> gameObj)
		: m_drag(0.f)
		, m_mass(0.f)
		, m_angularDrag(0.f)
		, m_maxForce({ FLOAT_MAX, FLOAT_MAX, FLOAT_MAX })
		, m_maxTorque({ FLOAT_MIN, FLOAT_MIN, FLOAT_MIN })
		, m_isKinematic(false)
		, m_passThrough(false)
		, m_usesGravity(false)
		, m_stopOnCollision(true)
		, m_reflectOnCollision(false)
		, m_clickable(true)
		, m_impartForce(false)
		, m_parent(gameObj)
		, m_collider(nullptr)
		, m_bodyModel(gameObj->m_model)
		, m_debugColor({ 0.0f, 1.0f, 0.0f })
		, m_force(0.f)
		, m_torque(0.f)
		, m_timeInterval(PhysicsManager::getInstance().time_interval)
	{ }

	void PhysicsBody::applyGravity(glm::vec3 dir, const float multiplier, float delta_time) {
		addForce(dir * multiplier);
	}

	void PhysicsBody::draw() {
		//m_collider->draw(m_debugColor);
	}


	void PhysicsBody::addTorque(glm::vec3 torque) {
		if (m_isKinematic)
			return;

		m_torque += torque;

		// Cap to maximum torque if needed
		//torque = std::min(torque, max_torque);
		//torque = std::max(torque, -max_torque);

		m_torque.x = std::min(m_torque.x, m_maxTorque.x);
		m_torque.y = std::min(m_torque.y, m_maxTorque.y);
		m_torque.z = std::min(m_torque.z, m_maxTorque.z);

		// Cap to minimum torque if needed
		m_torque.x = std::max(-m_maxTorque.x, m_torque.x);
		m_torque.y = std::max(-m_maxTorque.y, m_torque.y);
		m_torque.z = std::max(-m_maxTorque.z, m_torque.z);

	}

	void PhysicsBody::reverseForces(const float prop) {
		if (m_isKinematic)
			return;

		m_force *= -prop;
	}

	void PhysicsBody::impartForces(std::shared_ptr<PhysicsBody> other, const float massProp) {
		other->addForce(this->m_force * massProp);

		recalculateColliders();
	}

	void PhysicsBody::stopForces() {
		m_force = glm::vec3(0, 0, 0);
	}

	void PhysicsBody::reflectForces(glm::vec3 impulse) {
		glm::vec3 reflect(0,0, 0);

		reflect.x = impulse.x > 0 ? 1.f : -1.f;
		reflect.y = impulse.y > 0 ? 1.f : -1.f;
		reflect.z = impulse.z > 0 ? 1.f : -1.f;
		reflect.z = impulse.z > 0 ? 1.f : -1.f;

		m_force *= reflect;
	}

	void PhysicsBody::reduceForces(const float prop) {
		m_force *= prop;
	}

	void PhysicsBody::addForce(glm::vec3 dir, const float force) {
		addForce(dir * force);
	}

	void PhysicsBody::addForce(glm::vec3 incoming_force) {
		if (m_isKinematic)
			return;

		// TODO: double check if correct?
		incoming_force /= m_mass;
		if (glm::length2(incoming_force) < 0.0001)
			return;

		m_force += incoming_force;

		// Cap the force
		if (m_force.x > 0)
			m_force.x = std::min(m_force.x, m_maxForce.x);
		else
			m_force.x = std::max(m_force.x, -m_maxForce.x);
		if (m_force.y > 0)
			m_force.y = std::min(m_force.y, m_maxForce.y);
		else
			m_force.y = std::max(m_force.y, -m_maxForce.y);
		if (m_force.z > 0)
			m_force.z = std::min(m_force.z, m_maxForce.z);
		else
			m_force.z = std::max(m_force.z, -m_maxForce.z);

	}

	void PhysicsBody::recalculateColliders() {
		m_collider->recalculateBoundingBox();
	}

	void PhysicsBody::physicsTick(float delta_time) {
		std::vector<glm::vec2> toRemove;
		float rotx = m_torque.x * delta_time;
		float roty = m_torque.y * delta_time;
		float rotz = m_torque.z * delta_time;

		static auto get_sign_bit = [](float n) {
			return n == 0 ? 0 : (n > 0 ? 1 : -1);
		};

		static auto apply_drag = [&](float drag, int idx) {
			float val = m_torque[idx];
			m_torque[idx] -= get_sign_bit(val) * drag;
		};

		if (abs(m_torque.x) < m_angularDrag.x)
			m_torque.x = 0;
		else
			apply_drag(m_angularDrag.x, 0);
		if (abs(m_torque.y) < m_angularDrag.y)
			m_torque.y = 0;
		else
			apply_drag(m_angularDrag.y, 1);
		if (abs(m_torque.z) < m_angularDrag.z)
			m_torque.z = 0;
		else
			apply_drag(m_angularDrag.z, 2);

		// parent->model->rotate
		//m_bodyModel->rotate(m_torque.x, glm::vec3{1.0, 0.0, 0.0});
		//m_bodyModel->rotate(m_torque.y, glm::vec3{0.0, 1.0, 0.0});
		//m_bodyModel->rotate(m_torque.z, glm::vec3{0.0, 0.0, 1.0});
		m_bodyModel->translate(m_force * delta_time);
		
		m_bodyModel->rotate(rotx, m_bodyModel->m_forward);
		m_bodyModel->rotate(roty, m_bodyModel->m_up);
		m_bodyModel->rotate(rotz, m_bodyModel->m_right);


		float force_mag = glm::length(m_force);
		if (force_mag < m_drag) {
			stopForces();
		}
		else if (force_mag > 0) {
			// TODO: Make sure below is correct from force = (force / force_mag) * (force_mag - drag); to vvvvvvvv
			m_force = glm::normalize(m_force) * (force_mag - m_drag);
		}
	}

	void PhysicsBody::setBoxCollider() {
		auto handler = std::dynamic_pointer_cast<CollisionHandler>(m_parent);
		m_collider = std::make_shared<ColliderBox>(handler, m_bodyModel);
		m_collider->recalculateBoundingBox();
	}

	std::optional<glm::vec3> PhysicsBody::checkCollision(Ray& ray) {
		return m_collider->checkCollision(ray);
	}
}