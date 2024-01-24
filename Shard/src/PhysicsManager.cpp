#include "PhysicsManager.h"
#include "CollisionHandler.h"

#include <algorithm>
#include <list>
#include <queue>
#include <optional>

namespace Shard {
	PhysicsManager::PhysicsManager() {}

	PhysicsManager& PhysicsManager::getInstance() {
		static PhysicsManager instance;
		return instance;
	}

	void PhysicsManager::addPhysicsObject(PhysicsBody body) {
		// TODO: Use unordered_set, not vector
		//all_physics_objects.insert(body);
		
		auto iter = std::find(all_physics_objects.begin(), all_physics_objects.end(), body);

		if (iter == all_physics_objects.end())
			all_physics_objects.push_back(body);
		
	}

	void PhysicsManager::removePhysicsObject(PhysicsBody body) {
		// TODO: Use unordered_set, not vector
		//all_physics_objects.erase(body);
		
		auto iter = std::find(all_physics_objects.begin(), all_physics_objects.end(), body);
		
		if (iter != all_physics_objects.end()) {
			all_physics_objects.erase(iter);
		}
		
	}

	void PhysicsManager::clearList() {
		sap_x.clear();
	}

	void PhysicsManager::addToList(SAPEntry node) {
		sap_x.push_back(node);

		auto comparator = [](SAPEntry a, SAPEntry b) {
			return a.start > b.start;
		};

		std::sort(sap_x.begin(), sap_x.end(), comparator);
	}

	void PhysicsManager::reportCollisionsInAxis() {
		std::list<SAPEntry> active_objects;
		std::list<size_t> to_remove;
		

		auto equal = [](SAPEntry a, SAPEntry b) {
			return a.start == b.start && a.end == b.end && a.owner == b.owner;
		};

		for (const SAPEntry start : sap_x) {
			active_objects.push_back(start);

			auto iter = active_objects.begin();
			for (size_t i = 1; i < active_objects.size(); i++) {
				SAPEntry active = *(++iter);

				if (start == active)
					continue;
				/*
				* ^^^^^ should be better
				// TODO huh?
				if (equal(start, active))
					continue;
				*/

				if (start.start >= active.end)
					to_remove.push_back(i);
				else {
					CollidingObject col;

					if (start.owner->mass > active.owner->mass) {
						col.a = *start.owner;
						col.b = *active.owner;
					}
					else {
						col.a = *active.owner;
						col.b = *start.owner;
					}

					if (!findColliding(col.a, col.b))
						collisions_to_check_.insert(col);
				}
			}

			for (size_t i = to_remove.size(); i >= 0; i--) {
				auto iter = active_objects.begin();
				std::advance(iter, i);
				active_objects.erase(iter);
			}

			to_remove.clear();
		}
	}

	bool PhysicsManager::willTick() {
		// TODO: requires bootstrap

		return false;
	}

	bool PhysicsManager::update() {
		std::list<CollidingObject> to_remove;

		if (!willTick())
			return false;

		last_update = 0; // TODO: bootstrap get current millis;

		for (PhysicsBody &body : all_physics_objects) {
			if (body.uses_gravity)
				body.applyGravity(gravity_dir, gravity_modifier);

			body.physicsTick();
			body.recalculateColliders();
		}

		for (CollidingObject col : collidings_) {
			auto a_handler = col.a.coll_handler;
			auto b_handler = col.b.coll_handler;

			if (col.a.parent->to_be_destroyed_) {
				b_handler->onCollisionExit(nullptr);
				to_remove.push_back(col);
			}

			if (col.b.parent->to_be_destroyed_) {
				a_handler->onCollisionExit(nullptr);
				to_remove.push_back(col);
			}

			std::optional<glm::vec2> impulse = checkCollisionsBetweenObjects(col.a, col.b);

			if (impulse.has_value()) {
				a_handler->onCollisionStay(&col.b);
				b_handler->onCollisionStay(&col.a);
			}
			else {
				a_handler->onCollisionExit(&col.b);
				b_handler->onCollisionExit(&col.a);
				to_remove.push_back(col);
			}
		}

		for (CollidingObject& col_obj : to_remove) {
			collidings_.erase(col_obj);
		}

		to_remove.clear();
		checkForCollisions();

		return true;
	}

	void PhysicsManager::drawDebugColliders() {
		for (PhysicsBody &body : all_physics_objects)
			body.draw();
	}

	bool PhysicsManager::findColliding(PhysicsBody& a, PhysicsBody& b) {
		// TODO: Potentially flawed implementation.
		CollidingObject col { a, b };
		return collidings_.contains(col);
	}

	void PhysicsManager::broadPassSearchAndSweep() {

		std::list<PhysicsBody> candidates;
		for (PhysicsBody &body : all_physics_objects) {
			SAPEntry sx;
			glm::vec2 tmp = body.min_and_max_x;

			sx.owner = &body;
			sx.start = tmp.x;
			sx.end = tmp.y;

			addToList(sx);
		}

		reportCollisionsInAxis();
		clearList();
	}

	void PhysicsManager::broadPass() {
		broadPassSearchAndSweep();
		// broadPassBruteForce()
	}

	void PhysicsManager::narrowPass() {
		glm::vec2 impulse;
		std::optional<glm::vec2> possible_impulse;

		float mass_total, mass_a, mass_b, mass_prop = 0.f;

		for (CollidingObject col_obj : collisions_to_check_) {
			possible_impulse = checkCollisionsBetweenObjects(col_obj.a, col_obj.b);

			if (possible_impulse.has_value()) {
				impulse = possible_impulse.value();

				if (!col_obj.a.allows_pass_through && !col_obj.b.allows_pass_through) {
					mass_total = col_obj.a.mass + col_obj.b.mass;

					if (col_obj.a.is_kinematic)
						mass_prop = 1.f;
					else
						mass_prop = col_obj.a.mass / mass_total;

					if (col_obj.a.impart_force) {
						col_obj.a.impartForces(col_obj.b, mass_prop);
						col_obj.a.reduceForces(1.f - mass_prop);
					}

					mass_b = mass_prop;

					if (!col_obj.b.is_kinematic) {
						col_obj.b.parent->transform_.translate(-1 * impulse.x * mass_prop, -1 * impulse.y * mass_prop);
						mass_prop = 1.f - mass_prop;
					}
					else {
						mass_prop = 1.f;
					}

					mass_a = mass_prop;

					if (!col_obj.a.is_kinematic)
						col_obj.a.parent->transform_.translate(impulse.x * mass_prop, impulse.y * mass_prop);
					
					if (col_obj.a.stop_on_collision)
						col_obj.a.stopForces();
					if (col_obj.b.stop_on_collision)
						col_obj.b.stopForces();
				}

				col_obj.a.coll_handler->onCollisionEnter(&col_obj.b);
				col_obj.b.coll_handler->onCollisionEnter(&col_obj.a);

				collidings_.insert(col_obj);

				if (col_obj.a.reflect_on_collision)
					col_obj.a.reflectForces(impulse);
				if (col_obj.b.reflect_on_collision)
					col_obj.b.reflectForces(impulse);
			}
		}
	}

	void PhysicsManager::checkForCollisions() {
		broadPass();
		narrowPass();

		collisions_to_check_.clear();
	}

	std::optional<glm::vec2> PhysicsManager::checkCollisionsBetweenObjects(PhysicsBody& a, PhysicsBody& b) {

		std::optional<glm::vec2> impulse;
		for (auto *c1 : a.colliders) {
			for (auto *c2 : b.colliders) {
				impulse = c1->checkCollision(c2);
				if (impulse.has_value())
					return impulse.value();
			}
		}

		return std::nullopt;
	}
}