#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include "CollidingObject.h"
#include "SAPEntry.h"

#include <vector>
#include <list>
#include <unordered_set>
#include <optional>

namespace Shard {
	class SHARD_API PhysicsManager {
	public:
		long time_interval;

		static PhysicsManager& getInstance();

		PhysicsManager(PhysicsManager const&) = delete;
		PhysicsManager& operator=(PhysicsManager const&) = delete;

		void addPhysicsObject(PhysicsBody body);
		void removePhysicsObject(PhysicsBody body);
		void clearList();
		void addToList(SAPEntry node);
		// void outputList(SAPEntry node); only for debug, skipping
		void reportCollisionsInAxis();
		bool willTick();
		bool update();
		void drawDebugColliders();
		bool findColliding(PhysicsBody& a, PhysicsBody& b);
		void broadPassSearchAndSweep();
		void broadPass();

	private:

		struct ColObjComparator {
			bool operator()(CollidingObject& lhs, CollidingObject& rhs) {
				return 1;
				//return (lhs.a.equals(rhs.a)) && (lhs.b.equals(rhs.b));
			}
		};

		std::unordered_set<CollidingObject> collisions_to_check_;

		std::unordered_set<CollidingObject> collidings_;
		
		long last_update, last_debug_draw;
		//SAPEntry sap_x, sap_y;
		std::list<SAPEntry> sap_x;
		float gravity_modifier;
		glm::vec2 gravity_dir;

		std::vector<PhysicsBody> all_physics_objects;

		PhysicsManager();

		std::optional<glm::vec2> checkCollisionsBetweenObjects(PhysicsBody& a, PhysicsBody& b);
		//void broadPassBruteForce();
		void narrowPass();
		void checkForCollisions();
	};
}