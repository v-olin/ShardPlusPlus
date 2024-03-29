#pragma once

#include <vector>
#include <glm.hpp>
#include <memory>

#include "Collider.h"

namespace Shard {
	class GameObject;

	class PhysicsBody : public std::enable_shared_from_this<PhysicsBody> {
	public:
		float m_drag;
		float m_mass;

		glm::vec3 m_angularDrag;
		glm::vec3 m_maxForce;
		glm::vec3 m_maxTorque;

		bool m_isKinematic;
		bool m_passThrough;
		bool m_usesGravity;
		bool m_stopOnCollision;
		bool m_reflectOnCollision;
		bool m_impartForce;
		bool m_clickable;

		std::shared_ptr<GameObject> m_parent;
		std::shared_ptr<Collider> m_collider;
		std::shared_ptr<Model> m_bodyModel; // this is double pointy, very bad!!
		glm::vec3 m_debugColor;

		PhysicsBody();
		PhysicsBody(std::shared_ptr<GameObject> gameObj);

		void applyGravity(glm::vec3 dir, const float multiplier, float delta_time);
		void draw();
		void addTorque(glm::vec3 torque);
		void reverseForces(const float prop);
		void impartForces(std::shared_ptr<PhysicsBody> other, const float massProp);
		void stopForces();
		void reflectForces(glm::vec3 impulse);
		void reduceForces(const float prop);
		void addForce(glm::vec3 dir, const float force);
		void addForce(glm::vec3 dir);
		void recalculateColliders();
		void physicsTick(float delta_time);
		void setBoxCollider();
		std::optional<glm::vec3> checkCollision(Ray& ray);

		bool operator==(PhysicsBody& other){
			return m_parent == other.m_parent;
		}

		bool equals(PhysicsBody& other){
			return m_parent == other.m_parent;
		}

		bool equals(std::shared_ptr<PhysicsBody> other) {
			return m_parent == other->m_parent;
		}

	private:
		glm::vec3 m_force;
		glm::vec3 m_torque;
		long m_timeInterval;
	};


}