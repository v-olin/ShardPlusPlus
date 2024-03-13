#pragma once

#include "CollidingObject.h"
#include "SAPEntry.h"
#include "InputEvent.h"

#include <vector>
#include <list>
#include <unordered_set>
#include <optional>

namespace Shard {

	typedef struct Interval {
		Interval() = default;
		Interval(bool active, std::shared_ptr<PhysicsBody> body)
			: active(active)
			, gob_body(body) {}
		bool active{ false };
		int idx;
		std::shared_ptr<PhysicsBody> gob_body{ nullptr };
	} Interval;

	typedef struct IntervalEdge {
		IntervalEdge() = default;
		IntervalEdge(float val, bool is_b, std::shared_ptr<Interval> interval)
			: val(val)
			, is_b(is_b)
			, interval(interval) {}
		float val{ 0.0f };
		bool is_b{ true };
		std::shared_ptr<Interval> interval{ nullptr };
	} IntervalEdge;

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
		bool update(float delta_time);
		void drawDebugColliders();
		bool findColliding(std::shared_ptr<PhysicsBody> a, std::shared_ptr<PhysicsBody> b);

		std::vector<CollidingObject> sweepAndMotherfuckingPrune();

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

		std::vector<std::vector<int>> overlap_mat_x{ 0 };
		std::vector<std::vector<int>> overlap_mat_y{ 0 };
		std::vector<std::vector<int>> overlap_mat_z{ 0 };

		std::vector<CollidingObject> collisions{};
		int collision_last_frame{ 0 };

		const static int AXIS_X{ 0 };
		const static int AXIS_Y{ 1 };
		const static int AXIS_Z{ 2 };

		void updateEdgeList(int axis);
		void findOverlaps(int axis);
		void makeInterval(int axis, std::shared_ptr<PhysicsBody> gob_body);
		std::vector<std::shared_ptr<Interval>>& getInterval(int axis);
		std::vector<IntervalEdge>& getEdgeList(int axis);
		std::vector<std::vector<int>>& getOverlapMatrix(int axis);
		bool& getTraverseEdgeListBool(int axis);
		//int findIntervalIdx(int axis, std::shared_ptr<Interval> interval);
		void resetIdx(int axis);
		void BubbleSort(int axis);
		void TraverseEdgeList(int axis);

		////////////////////////////////////////

		long long last_update{ 0 }, last_debug_draw{ 0 };
		std::list<SAPEntry> sap_x;
		glm::vec3 gravity_dir{ 0.0f, -1.0f, 0.0f };

		std::vector<std::shared_ptr<PhysicsBody>> all_physics_objects;

		PhysicsManager();

		std::optional<glm::vec3> getImpulseFromCollision(std::shared_ptr<PhysicsBody> a, std::shared_ptr<PhysicsBody> b);

		void runCollisionReaction(glm::vec3 impulse, CollidingObject col_obj);
		void runCollisionCheck();
		void checkForCollisions();


		std::optional<std::shared_ptr<PhysicsBody>> getClickedBody(InputEvent ie);
		std::optional<glm::vec3> clickHitsBody(InputEvent ie, std::shared_ptr<PhysicsBody> body);
		Ray getRayFromClick(InputEvent ie);
	private:
	}; // end class
	
} // end namespace shard