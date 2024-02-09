#pragma once

#include "CollidingObject.h"
#include "SAPEntry.h"

#include <vector>
#include <list>
#include <unordered_set>
#include <optional>

namespace Shard {

	class  PhysicsManager {
	public:
		long time_interval;
		float gravity_modifier;

		static PhysicsManager& getInstance();

		PhysicsManager(PhysicsManager const&) = delete;
		PhysicsManager& operator=(PhysicsManager const&) = delete;

		void addPhysicsObject(std::shared_ptr<PhysicsBody> body);
		void removePhysicsObject(std::shared_ptr<PhysicsBody> body);
		bool willTick();
		bool update();
		void drawDebugColliders();
		bool findColliding(std::shared_ptr<PhysicsBody> a, std::shared_ptr<PhysicsBody> b);

		std::vector<CollidingObject> sweepAndMotherfuckingPrune();


		typedef struct Interval {
			Interval() = default;
			Interval(bool active, std::shared_ptr<PhysicsBody> body)
				: active(active)
				, gob_body(body){}
			bool active{ false };
			std::shared_ptr<PhysicsBody> gob_body{ nullptr };
		} Interval;

		typedef struct IntervalEdge {
			IntervalEdge() = default;
			IntervalEdge(float val, bool is_b, std::shared_ptr<Interval> interval)
				: val(val)
				, is_b(is_b)
				, interval(interval) {}
			float val{0.0f};
			bool is_b{true};
			std::shared_ptr<Interval> interval{ nullptr };
		} IntervalEdge;

		std::vector<std::shared_ptr<Interval>> all_x_intervals{};
		std::vector<std::shared_ptr<Interval>> all_y_intervals{};
		std::vector<std::shared_ptr<Interval>> all_z_intervals{};
		std::vector<std::shared_ptr<Interval>> active_interval_list{};

		std::vector<IntervalEdge> edge_list_x{};
		std::vector<IntervalEdge> edge_list_y{};
		std::vector<IntervalEdge> edge_list_z{};

		bool traverse_edge_list_x{ true };
		bool traverse_edge_list_y{ true };
		bool traverse_edge_list_z{ true };

		std::vector<std::vector<bool>> overlap_mat_x{ false };
		std::vector<std::vector<bool>> overlap_mat_y{false};
		std::vector<std::vector<bool>> overlap_mat_z{false};

		std::vector<CollidingObject> collisions{};

		long long last_update{ 0 }, last_debug_draw{ 0 };
		std::list<SAPEntry> sap_x;
		glm::vec3 gravity_dir{ 0.0f, -1.0f, 0.0f };

		std::vector<std::shared_ptr<PhysicsBody>> all_physics_objects;

		PhysicsManager();

		std::optional<glm::vec3> getImpulseFromCollision(std::shared_ptr<PhysicsBody> a, std::shared_ptr<PhysicsBody> b);

		void runCollisionCheck();
		void checkForCollisions();
	private:
	}; // end class
	
} // end namespace shard