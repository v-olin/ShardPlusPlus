#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <vector>
#include <SDL.h>
#include <glm.hpp>

#include "Collider.h"
#include "GameObject.h"

namespace Shard {
	class SHARD_API Transform;

	class SHARD_API PhysicsBody {
	public:
		float angular_drag;
		float drag;
		float mass;
		glm::vec2 min_and_max_x;
		glm::vec2 min_and_max_y;
		float max_force;
		float max_torque;
		bool is_kinematic;
		bool allows_pass_through;
		bool uses_gravity;
		bool stop_on_collision;
		bool reflect_on_collision;
		bool impart_force;
		GameObject* parent;
		std::vector<ColliderCircle*> circ_colliders; 
		std::vector<ColliderRect*> rect_colliders; 
		CollisionHandler *coll_handler;

		PhysicsBody() {}
		PhysicsBody(GameObject* game_obj);

		// change to Vec3 for 3d
		void applyGravity(glm::vec2 dir, const float multiplier);
		void draw();
		glm::vec2 getMinAndMax(const bool x);
		void addTorque(const float dir);
		void reverseForces(const float prop);
		void impartForces(PhysicsBody other, const float massProp);
		void stopForces();
		void reflectForces(glm::vec2 impulse);
		void reduceForces(const float prop);
		void addForce(glm::vec2 dir, const float force);
		void addForce(glm::vec2 dir);
		void recalculateColliders();
		void physicsTick();
		void addRectCollider();
		void addRectCollider(int x, int y, int w, int h);
		void addCircleCollider();
		void addCircleCollider(int x, int y, int rad);
		//void addCollider(Collider coll);
		std::optional<glm::vec2> checkCollisions(glm::vec2 other);

		bool operator==(const PhysicsBody& other) {
			return parent == other.parent;
		}

	private:
		float torque_;
		glm::vec2 force_;
		float time_interval_;
		SDL_Color debug_color_;
		Transform3D trans;
		std::vector<Collider> collision_candidates;
	};
}