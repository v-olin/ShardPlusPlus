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
		void clearList();
		void addToList(SAPEntry node);
		// void outputList(SAPEntry node); only for debug, skipping
		void reportCollisionsInAxis();
		bool willTick();
		bool update();
		void drawDebugColliders();
		bool findColliding(std::shared_ptr<PhysicsBody> a, std::shared_ptr<PhysicsBody> b);
		void broadPassSearchAndSweep();
		void broadPass();

	private:

		std::vector<CollidingObject> collisions_to_check_;
		std::vector<CollidingObject> collidings_;

		
		long last_update, last_debug_draw;
		//SAPEntry sap_x, sap_y;
		std::list<SAPEntry> sap_x;
		glm::vec2 gravity_dir;

		std::vector<std::shared_ptr<PhysicsBody>> all_physics_objects;

		PhysicsManager();

		std::optional<glm::vec2> checkCollisionsBetweenObjects(std::shared_ptr<PhysicsBody> a, std::shared_ptr<PhysicsBody> b);
		//void broadPassBruteForce();
		void narrowPass();
		void checkForCollisions();
	};
	
	//int compareSAP(Shard::SAPEntry& a, Shard::SAPEntry& b) {
	//	return a.start - b.start;
	//}
}
