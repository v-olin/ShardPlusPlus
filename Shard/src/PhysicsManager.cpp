#include "PhysicsManager.h"
#include "CollisionHandler.h"
#include "Bootstrap.h"
#include "Logger.h"

#include <algorithm>
#include <list>
#include <queue>
#include <optional>

namespace Shard {

	PhysicsManager::PhysicsManager() {

	}

	PhysicsManager& PhysicsManager::getInstance() {
		static PhysicsManager instance;
		return instance;
	}

	void PhysicsManager::addPhysicsObject(std::shared_ptr<PhysicsBody> body) {
		// TODO: std::find? yea.
		bool exists = false;
		for(size_t i = 0; i < all_physics_objects.size(); i++){
			auto other = all_physics_objects.at(i);
			if (other->parent == body->parent){
				exists = true;
				break;
			}
		}
		if (!exists) {
			all_physics_objects.push_back(body);

			auto [min_max_x, min_max_y, min_max_z] = body->collider->getMinMaxDims();

			auto interval_x = std::make_shared<Interval>(false, body);
			all_x_intervals.push_back(interval_x);
			IntervalEdge b_x{ min_max_x[0], true, interval_x };
			IntervalEdge e_x{ min_max_x[1], false, interval_x };
			
			//TODD maybe insertion sort, or maybe bubble sort + insertion sort, and set flag to tell SAP to traverse list
			//: Potentially: https://stackoverflow.com/a/25524075/23276969
			edge_list_x.push_back(b_x);
			edge_list_x.push_back(e_x);
			traverse_edge_list_x = true;
	
			int n = overlap_mat_x.size();
			std::vector<bool> new_row(n + 1, false);
			overlap_mat_x.push_back(new_row);
			for (size_t i = 0; i < n; i++)
				overlap_mat_x[i].push_back(false);
		}
	}

	void PhysicsManager::removePhysicsObject(std::shared_ptr<PhysicsBody> body) {
		for(size_t i = 0; i < all_physics_objects.size(); i++){
			auto other = all_physics_objects.at(i);
			if(other->parent == body->parent){
				auto iter = all_physics_objects.begin();
				std::advance(iter, i);
				all_physics_objects.erase(iter);
			}
		}
	}

	bool PhysicsManager::willTick() {
		// TODO: requires bootstrap
		return true;
		return Bootstrap::getCurrentMillis() - last_update > time_interval;
	}

	bool PhysicsManager::update() {
		std::list<size_t> to_remove;

		if (!willTick())
			return false;

		last_update = Bootstrap::getCurrentMillis();

		for (auto &body : all_physics_objects) {
			if (body->uses_gravity)
				body->applyGravity(gravity_dir, gravity_modifier);

			body->physicsTick();
			body->recalculateColliders();
		}

		int i = 0;
		bool remove = false;
		for (CollidingObject& col : collisions) {

			// todo: if decide to translate, don't do that for kinematic objects

			//This is bad since the parents could have been deleted by the GameObjectManager
			// upcast to shared_ptr
			auto a_handler = std::dynamic_pointer_cast<CollisionHandler>(col.a->parent);
			auto b_handler = std::dynamic_pointer_cast<CollisionHandler>(col.b->parent);

			if (!a_handler || !b_handler) {
				Logger::log("failed upcast", LOG_LEVEL_ALL);
			}

			if (col.a->parent->to_be_destroyed_) {
				if (!col.b->parent->to_be_destroyed_)
					b_handler->onCollisionExit(nullptr);
				to_remove.push_back(i++);
				continue;
			}

			if (col.b->parent->to_be_destroyed_) {
				if (!col.a->parent->to_be_destroyed_)
					a_handler->onCollisionExit(nullptr);
				to_remove.push_back(i++);
				continue;
			}

			
			std::optional<glm::vec2> impulse = getImpulseFromCollision(col.a, col.b);

			if (impulse.has_value()) {
				a_handler->onCollisionStay(col.b);
				b_handler->onCollisionStay(col.a);
			}
			else {
				a_handler->onCollisionExit(col.b);
				b_handler->onCollisionExit(col.a);
				remove = true;
			}
			if (remove)
				to_remove.push_back(i++);
			remove = false;

		}

		// TODO: Batch erase instead of single
		// Use: erase-remove idiom
		int removed = 0;
		for (size_t idx : to_remove) {
			auto iter = collisions.begin();
			std::advance(iter, idx-removed);
			collisions.erase(iter);
			removed++;
		}

		checkForCollisions();
		return true;
	}

	void PhysicsManager::drawDebugColliders() {
		for (const std::shared_ptr<PhysicsBody>& body : all_physics_objects)
			body->draw();
	}

	bool PhysicsManager::findColliding(std::shared_ptr<PhysicsBody> a, std::shared_ptr<PhysicsBody> b) {
		CollidingObject col { a, b };

		for (auto &col_ : collisions) {
			if (col.equals(col_))
				return true;
		}

		return false;
	}

	std::vector<CollidingObject> PhysicsManager::sweepAndMotherfuckingPrune() {



		for(size_t i = 0; i < edge_list_x.size(); i++){
			auto interval = edge_list_x[i].interval;
			auto [min_max_x, min_max_y, min_max_z] = interval->gob_body->collider->getMinMaxDims();
			auto val = edge_list_x[i].is_b ? min_max_x[0] : min_max_x[1];
			edge_list_x[i].val = val;
		}

		//auto [min_max_x, min_max_y, min_max_z] = body->collider->getMinMaxDims();

		std::vector<CollidingObject> collisions;

		static auto findIntervalNum = [&](Interval& interval) {
			for (size_t i = 0; i < all_x_intervals.size(); i++)
				if (all_x_intervals[i]->gob_body == interval.gob_body)
					return (int)i;
			return -1;
			};

		//simple/nice case
		if (!traverse_edge_list_x) {

			//list is not sorted, interval values are updated
			size_t n = edge_list_x.size();
			for (size_t i = 0; i < n - 1; i++) {
				for (size_t j = 0; j < n - 1 - i; j++) {
					if (edge_list_x[j + 1].val < edge_list_x[j].val) {
						int interval_a = findIntervalNum(*edge_list_x[j].interval);
						int interval_b = findIntervalNum(*edge_list_x[j + 1].interval);

						if (interval_a == -1 || interval_b == -1) {
							Logger::log("oh fuck...(inside sweepAndMotherFuckingPrune\n");
							std::exit(1);
						}
						auto a = std::min(interval_a, interval_b);
						auto b = std::max(interval_a, interval_b);
						overlap_mat_x[a][b] = overlap_mat_x[a][b] ^ true;


						auto tmp = edge_list_x[j];
						edge_list_x[j] = edge_list_x[j + 1];
						edge_list_x[j + 1] = tmp;
					}
				}
			}

		}
		else {
			//sort list of edges, bubble
			size_t n = edge_list_x.size();
			for (size_t i = 0; i < n - 1; i++) {
				for (size_t j = 0; j < n - 1 - i; j++) {
					if (edge_list_x[j + 1].val < edge_list_x[j].val) {
						auto tmp = edge_list_x[j];
						edge_list_x[j] = edge_list_x[j + 1];
						edge_list_x[j + 1] = tmp;
					}
				}
			}

			//Traverse edge_list_x from start to end
			for (size_t i = 0; i < edge_list_x.size(); i++) {
				auto interval_edge = edge_list_x[i];
				if (interval_edge.is_b) {
					//When a b is encounted, mark corresponding object interval as active in an active_interval_list
					interval_edge.interval->active = true;
					active_interval_list.push_back(interval_edge.interval);
					if (active_interval_list.size() > 1) {
						// If more than one interval in active_interval_list, then update the overlap_mat_x in right position
						for (size_t j = 0; j < active_interval_list.size(); j++) {
							auto fst = active_interval_list[j];
							for (size_t k = j; k < active_interval_list.size(); k++) {
								auto snd = active_interval_list[k];
								auto interval_a = findIntervalNum(*fst);
								auto interval_b = findIntervalNum(*snd);
								auto a = std::min(interval_a, interval_b);
								auto b = std::max(interval_a, interval_b);
								overlap_mat_x[a][b] = true;
							}
						}

					}
				} else {

					//When an e is encountered, delete the interval in active_interval_list
					active_interval_list.erase(
						std::remove_if(
							active_interval_list.begin(), 
							active_interval_list.end(), 
							[&](std::shared_ptr<Interval> interval) {
								return interval->gob_body == interval_edge.interval->gob_body;
							}
						), active_interval_list.end()
					);

				}
			}
		}

		size_t rows = overlap_mat_x.size();
		size_t cols = overlap_mat_x[0].size();
		for (size_t i = 0 ; i < rows; i++) {
			for (size_t j = i + 1; j < cols; j++) {
				bool overlap_x = overlap_mat_x[i][j];
				//bool overlap_y = overlap_mat_y[i][j];
				bool overlap_y = true;
				//bool overlap_z = overlap_mat_y[i][j];
				bool overlap_z = true;
					
				if (overlap_x && overlap_y && overlap_z) {
					CollidingObject col;
					auto gob_body_a = all_x_intervals[i]->gob_body;
					auto gob_body_b = all_x_intervals[j]->gob_body;

					if (gob_body_a->mass > gob_body_b->mass) {
						col.a = gob_body_a;
						col.b = gob_body_b;
					}
					else {
						col.a = gob_body_b;
						col.b = gob_body_a;
					}
					if (!findColliding(col.a, col.b)) 
						collisions.push_back(col);

				}
			}
		}
		
		return collisions;

	}

	void PhysicsManager::runCollisionCheck() {

		/*
		1. call sweep and prune, pupulate collisions to check with ACTUALL coillisions

		2. then what?	
			Loop over collisions, calculate the impulse between them, and use same "physics" that is already implemented in the loop
		*/

		glm::vec3 impulse;
		std::optional<glm::vec3> possible_impulse;
		float mass_total, mass_prop = 0.f;


		collisions = sweepAndMotherfuckingPrune();
		for (CollidingObject &col_obj : collisions) {
			possible_impulse = getImpulseFromCollision(col_obj.a, col_obj.b);

			if (possible_impulse.has_value()) {
				impulse = possible_impulse.value();

				if (!col_obj.a->pass_through && !col_obj.b->pass_through) {
					mass_total = col_obj.a->mass + col_obj.b->mass;

					if (col_obj.a->is_kinematic)
						mass_prop = 1.f;
					else
						mass_prop = col_obj.a->mass / mass_total;

					if (col_obj.a->impart_force) {
						col_obj.a->impartForces(col_obj.b, mass_prop);
						col_obj.a->reduceForces(1.f - mass_prop);
					}

					if (!col_obj.b->is_kinematic) {
						glm::vec3 force{ -impulse.x, -impulse.y, -impulse.z};
						col_obj.b->parent->body_->trans->translate(force * mass_prop);
						mass_prop = 1.f - mass_prop;
					}
					else {
						mass_prop = 1.f;
					}

					if (!col_obj.a->is_kinematic) {
						glm::vec3 force{ impulse.x, impulse.y, impulse.z};
						col_obj.a->parent->body_->trans->translate(force * mass_prop);
					}
					
					if (col_obj.a->stop_on_collision)
						col_obj.a->stopForces();

					if (col_obj.b->stop_on_collision)
						col_obj.b->stopForces();



					if (col_obj.a->reflect_on_collision)
						col_obj.a->reflectForces(impulse);
					if (col_obj.b->reflect_on_collision)
						col_obj.b->reflectForces(impulse);

				}

				//col_obj.a->parent->onCollisionEnter(&col_obj.b);
				//col_obj.b->coll_handler->onCollisionEnter(&col_obj.a);
				(std::dynamic_pointer_cast<CollisionHandler>(col_obj.a->parent))->onCollisionEnter(col_obj.b);
				(std::dynamic_pointer_cast<CollisionHandler>(col_obj.b->parent))->onCollisionEnter(col_obj.a);


			}
		}
	}

	void PhysicsManager::checkForCollisions() {
		runCollisionCheck();
		collisions.clear();
	}

	std::optional<glm::vec3> PhysicsManager::getImpulseFromCollision(std::shared_ptr <PhysicsBody> a, std::shared_ptr<PhysicsBody> b) {
		
		if (a->parent->to_be_destroyed_ || b->parent->to_be_destroyed_)
			return std::nullopt;

		// Check penetration level (sus)
		auto [min_max_x_a, min_max_y_a, min_max_z_a] = a->collider->getMinMaxDims();
		auto [min_max_x_b, min_max_y_b, min_max_z_b] = b->collider->getMinMaxDims();
		
		// todo: pray that one object is never contained in the other
		// otherwise we're PROBABLY completely fucked
		/*
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣤⣤⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣴⣿⣿⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⡿⠿⣿⣿⣿⣿⡿⠿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⢀⣴⣦⡈⠻⣦⣤⣿⣿⣧⣤⣶⠏⢀⣦⣄⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⢀⣴⣿⣿⣿⣷⣤⣈⠙⠛⠛⠛⢉⣠⣴⣿⣿⣿⣷⣄⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⢠⣿⣿⣿⣿⠟⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⢻⣿⣿⣿⣆⠀⠀⠀⠀
			⠀⠀⠀⢀⣿⣿⣿⣿⠃⣰⣿⣿⡿⠛⠋⠉⠛⠻⣿⣿⣷⡀⠹⣿⣿⣿⡆⠀⠀⠀
			⠀⠀⠀⣸⣿⣿⣿⠃⣰⣿⣿⠋⣠⣾⡇⢸⣷⣦⠈⣿⣿⣿⡄⢹⣿⣿⣿⠀⠀⠀
			⠀⠀⠀⣿⣿⣿⠋⠀⠉⠉⠉⠀⣿⣿⡇⢸⣿⣿⡇⠉⠉⠉⠁⠀⢻⣿⣿⡆⠀⠀
			⠀⠀⢰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀
			⠀⠀⠀⠙⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠃⠘⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠁⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠈⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠃⠀⠀⠀⠀⠀⠀⠀⠀
		*/

		float overlap_x{ 0 };
		if (min_max_x_a[0] < min_max_x_b[0])
			overlap_x = min_max_x_a[1] - min_max_x_b[0];
		else
			overlap_x = min_max_x_b[1] - min_max_x_a[0];

		float overlap_y{ 0 };
		if (min_max_y_a[0] < min_max_y_b[0])
			overlap_y = min_max_y_a[1] - min_max_y_b[0];
		else
			overlap_y = min_max_y_b[1] - min_max_y_a[0];

		float overlap_z{ 0 };
		if (min_max_z_a[0] < min_max_z_b[0])
			overlap_z = min_max_z_a[1] - min_max_z_b[0];
		else
			overlap_z = min_max_z_b[1] - min_max_z_a[0];

		//auto min_overlap = std::min(overlap_x, std::min(overlap_y, overlap_z));
		auto min_overlap = std::min({ overlap_x, overlap_y, overlap_z });

		if (overlap_x == min_overlap)
			return std::optional<glm::vec3>({ overlap_x, 0.0f, 0.0f });
		else if (overlap_y == min_overlap)
			return std::optional<glm::vec3>({ 0.0f, overlap_y, 0.0f });
		else if (overlap_z == min_overlap)
			return std::optional<glm::vec3>({ 0.0f, 0.0f, overlap_z });
		
		// uuuuuuuuuuuuuuhhhh.............. no penetration... but collided?
		// 100 megawhat?
		return std::nullopt;
		
		
		
		
		
		
		/*
		If above code is not sufficient, maybe implement this?
		if x == max(x, y, z)
			compare x coords between a and b to figure out what dir to push them
			ie if a.x < b.x push a in -x dir and b in +x dir
		*/

	}

}