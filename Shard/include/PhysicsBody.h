#pragma once

#include <vector>
#include <SDL.h>
#include <glm.hpp>
#include <memory>

#include "Collider.h"
#include "Transform3D.h"

namespace Shard {
	class GameObject;

	class PhysicsBody : public std::enable_shared_from_this<PhysicsBody> {
	public:
		float angular_drag;
		float drag;
		float mass;
		glm::vec2 min_and_max_x;
		glm::vec2 min_and_max_y;
		float max_force;
		float max_torque;
		bool is_kinematic;
		bool pass_through;
		bool uses_gravity;
		bool stop_on_collision;
		bool reflect_on_collision;
		bool impart_force;
		std::shared_ptr<GameObject> parent;
		//std::vector<Collider*> colliders;
		std::vector<std::shared_ptr<Collider>> colliders;
		SDL_Color debug_color_{0,255,0,255};
		//CollisionHandler* coll_handler;
		std::shared_ptr<Transform3D> trans;

		PhysicsBody();
		PhysicsBody(std::shared_ptr<GameObject> game_obj);

		// change to Vec3 for 3d
		void applyGravity(glm::vec2 dir, const float multiplier);
		void draw();
		glm::vec2 getMinAndMax(const bool x);
		void addTorque(const float dir);
		void reverseForces(const float prop);
		void impartForces(std::shared_ptr<PhysicsBody> other, const float massProp);
		void stopForces();
		void reflectForces(glm::vec2 impulse);
		void reduceForces(const float prop);
		void addForce(glm::vec2 dir, const float force);
		void addForce(glm::vec2 dir);
		void recalculateColliders();
		void physicsTick();
		void addRectCollider();
		void addRectCollider(float x, float y, float w, float h);
		void addCircleCollider();
		void addCircleCollider(float x, float y, float rad);
		//void addCollider(Collider coll);
		std::optional<glm::vec2> checkCollisions(glm::vec2 other);

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
		float torque_;
		glm::vec2 force_;
		long time_interval_;
		std::vector<Collider*> collision_candidates;
	};
}