#pragma once

#include <vector>
#include <glm.hpp>
#include <memory>

#include "Collider.h"
#include "Transform.h"

namespace Shard {
	class GameObject;

	class PhysicsBody : public std::enable_shared_from_this<PhysicsBody> {
	public:
		float drag{ 0.0f };
		float mass{ 0.0f };
		
		glm::vec3 angular_drag{ 0.0f };
		glm::vec3 max_force { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
		glm::vec3 max_torque{ std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min() };

		bool is_kinematic{ false };
		bool pass_through{ false };
		bool uses_gravity{ false };
		bool stop_on_collision{ true };
		bool reflect_on_collision{ false };
		bool impart_force{ false };

		std::shared_ptr<GameObject> parent{ nullptr };
		//std::vector<Collider*> colliders;
		std::shared_ptr<Collider> collider{nullptr};
		glm::vec3 debug_color{ 1.0f, 1.0f, 1.0f };
		//CollisionHandler* coll_handler;
		std::shared_ptr<Transform> trans;

		PhysicsBody();
		PhysicsBody(std::shared_ptr<GameObject> game_obj);

		// change to Vec3 for 3d
		void applyGravity(glm::vec3 dir, const float multiplier);
		void draw();
		glm::vec2 getMinAndMax(const bool x);
		void addTorque(const float dir);
		void reverseForces(const float prop);
		void impartForces(std::shared_ptr<PhysicsBody> other, const float massProp);
		void stopForces();
		void reflectForces(glm::vec3 impulse);
		void reduceForces(const float prop);
		void addForce(glm::vec3 dir, const float force);
		void addForce(glm::vec3 dir);
		void recalculateColliders();
		void physicsTick();
		void setBoxCollider();
		//void setBoxCollider(float x, float y, float z, float w, float h, float d);
		std::optional<glm::vec2> checkCollision(Ray& ray);

		bool operator==(PhysicsBody& other){
			return parent == other.parent;
		}

		bool equals(PhysicsBody& other){
			return parent == other.parent;
		}

		bool equals(std::shared_ptr<PhysicsBody> other) {
			return parent == other->parent;
		}

	private:
		glm::vec3 force{ 0.0f };
		glm::vec3 torque{ 0.0f };
		long time_interval_{0};
	};

}