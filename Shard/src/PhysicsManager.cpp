#include "PhysicsManager.h"
#include "CollisionHandler.h"
#include "Bootstrap.h"
#include "Logger.h"

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

	void PhysicsManager::addPhysicsObject(PhysicsBody* body) {
		// TODO: std::find?
		bool exists = false;
		for(size_t i = 0; i < all_physics_objects.size(); i++){
			auto other = all_physics_objects.at(i);
			if (other->parent == body->parent){
				exists = true;
				break;
			}
		}
		if (!exists)
			all_physics_objects.push_back(body);
	}

	void PhysicsManager::removePhysicsObject(PhysicsBody* body) {
		for(size_t i = 0; i < all_physics_objects.size(); i++){
			auto other = all_physics_objects.at(i);
			if(other->parent == body->parent){
				auto iter = all_physics_objects.begin();
				std::advance(iter, i);
				all_physics_objects.erase(iter);
			}
		}
	}

	void PhysicsManager::clearList() {
		sap_x.clear();
	}

	void PhysicsManager::addToList(SAPEntry node) {
		sap_x.push_back(node);

		//hopefully the values in the list dont change, if they do, implement bubble sort.
		SAPEntry lhs = *(sap_x.begin());

		sap_x.sort([](const SAPEntry& lhs, const SAPEntry& rhs) {
			return lhs.start < rhs.start;
		});

		/*
		// bubble sort
		for(size_t i = 0; i < sap_x.size() - 1; i++){
			auto iter = sap_x.begin();
			for (size_t j = 0; j < sap_x.size() - 1 - i; ++j) {
				auto curr = iter, next = ++iter;
				if ((*curr).start > (*next).start) {
					auto tmp = curr;
					curr = next;
					next = tmp;
				}
			}
		}
		*/
	}

	void PhysicsManager::reportCollisionsInAxis() {
		std::vector<SAPEntry> active_objects;

		//std::vector<size_t> to_remove;

		auto equal = [](SAPEntry a, SAPEntry b) {
			return a.start == b.start && a.end == b.end && a.owner == b.owner;
		};

		for (const SAPEntry start : sap_x) {
			active_objects.push_back(start);

			auto iter = active_objects.begin();
			for (size_t i = 0; i < active_objects.size(); i++) {
				SAPEntry active = active_objects[i];
				//SAPEntry active = *(++iter);
				
				// Ensures an object can't collide with itself
				if (equal(start, active))
					continue;
				
				if (start.start >= active.end)
					//to_remove.push_back(i);
					int x = 0; // <-- dummy instruction
				else {
					// Collision!!!!, not actually a collision

				/*	if (start.owner->parent->hasTag("Bullet")) {
 						Logger::log("[IF] ACTIVE: " + active.owner->parent->getTags(), LOG_LEVEL_ALL);
					} else if (active.owner->parent->hasTag("Bullet")) {
						Logger::log("[ELSE IF] START: " + start.owner->parent->getTags(), LOG_LEVEL_ALL);
					}*/

					CollidingObject col;
				
					if (start.owner->mass > active.owner->mass) {
						col.a = *start.owner;
						col.b = *active.owner;
					}
					else {
						col.a = *active.owner;
						col.b = *start.owner;
					}
				
					if (!findColliding(col.a, col.b)) {
						collisions_to_check_.push_back(col);
					}
				}
			}

			/*
			for (auto riter = to_remove.rbegin(); riter != to_remove.rend(); ++riter) {
				auto act_iter = active_objects.begin();
				std::advance(act_iter, *riter);
				active_objects.erase(act_iter);
			}
			*/

			//to_remove.clear();
		}
	}

	bool PhysicsManager::willTick() {
		// TODO: requires bootstrap
		return Bootstrap::getCurrentMillis() - last_update > time_interval;
	}

	bool PhysicsManager::update() {
		std::list<size_t> to_remove;

		if (!willTick())
			return false;

		// TODO: bootstrap get current millis;
		last_update = 0;

		for (PhysicsBody* body : all_physics_objects) {
			if (body->uses_gravity)
				body->applyGravity(gravity_dir, gravity_modifier);

			body->physicsTick();
			body->recalculateColliders();
		}

		int i = 0;
		bool remove = false;
		for (CollidingObject& col : collidings_) {

			auto a_handler = dynamic_cast<CollisionHandler*>(col.a.parent);
			auto b_handler = dynamic_cast<CollisionHandler*>(col.b.parent);

			if (col.a.parent->to_be_destroyed_) {
				b_handler->onCollisionExit(nullptr);
				remove = true;
				//to_remove.push_back(i);
			}

			if (col.b.parent->to_be_destroyed_) {
				a_handler->onCollisionExit(nullptr);
				remove = true;
				//to_remove.push_back(i);
			}

			std::optional<glm::vec2> impulse = checkCollisionsBetweenObjects(col.a, col.b);

			if (impulse.has_value()) {
				a_handler->onCollisionStay(&col.b);
				b_handler->onCollisionStay(&col.a);
			}
			else {
				a_handler->onCollisionExit(&col.b);
				b_handler->onCollisionExit(&col.a);
				remove = true;
				//to_remove.push_back(i);
			}
			if (remove)
				to_remove.push_back(i++);
			remove = false;

		}

		int removed = 0;
		for (size_t idx : to_remove) {
			auto iter = collidings_.begin();
			std::advance(iter, idx-removed);
			collidings_.erase(iter);
			removed++;
		}

		to_remove.clear();
		checkForCollisions();

		return true;
	}

	void PhysicsManager::drawDebugColliders() {
		for (PhysicsBody* body : all_physics_objects)
			body->draw();
	}

	bool PhysicsManager::findColliding(PhysicsBody& a, PhysicsBody& b) {
		// TODO: Potentially flawed implementation.
		CollidingObject col { a, b };

		for (auto &col_ : collidings_) {
			if (col == col_)
				return true;
		}

		return false;
	}

	void PhysicsManager::broadPassSearchAndSweep() {
		for (PhysicsBody* body : all_physics_objects) {
			SAPEntry sx;
			glm::vec2 tmp = body->min_and_max_x;

			sx.owner = body;
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

				if (!col_obj.a.pass_through && !col_obj.b.pass_through) {
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
						col_obj.b.parent->body_->trans.translate(-1 * impulse.x * mass_prop, -1 * impulse.y * mass_prop);
						mass_prop = 1.f - mass_prop;
					}
					else {
						mass_prop = 1.f;
					}

					mass_a = mass_prop;

					if (!col_obj.a.is_kinematic)
						col_obj.a.parent->body_->trans.translate(impulse.x * mass_prop, impulse.y * mass_prop);
					
					if (col_obj.a.stop_on_collision)
						col_obj.a.stopForces();
					if (col_obj.b.stop_on_collision)
						col_obj.b.stopForces();
				}

				//col_obj.a.parent->onCollisionEnter(&col_obj.b);
				//col_obj.b.coll_handler->onCollisionEnter(&col_obj.a);
				(dynamic_cast<CollisionHandler*>(col_obj.a.parent))->onCollisionEnter(&col_obj.b);
				(dynamic_cast<CollisionHandler*>(col_obj.b.parent))->onCollisionEnter(&col_obj.a);

				collidings_.push_back(col_obj);

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