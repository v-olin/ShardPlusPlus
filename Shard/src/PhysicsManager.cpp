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

	void PhysicsManager::makeInterval(int axis, std::shared_ptr<PhysicsBody> gob_body) {

		// Check penetration level (sus)
		auto min_max = gob_body->collider->getMinMaxDims();
		auto& min_max_x = min_max[0];
		auto& min_max_y = min_max[1];
		auto& min_max_z = min_max[2];

		glm::vec2 min_max_vals{};
		auto& edge_list = getEdgeList(axis);
		auto& overlap_matrix = getOverlapMatrix(axis);
		auto& intervals = getInterval(axis);
		
		if (axis == AXIS_X) {
			min_max_vals = min_max_x;
			traverse_edge_list_x = true;
		}
		else if (axis == AXIS_Y) {
			min_max_vals = min_max_y;
			traverse_edge_list_y = true;
		}
		else { // assuming z, not safe
			min_max_vals = min_max_z;
			traverse_edge_list_z = true;
		}

		auto interval = std::make_shared<Interval>(false, gob_body);
		intervals.push_back(interval);
		IntervalEdge b{ min_max_vals[0], true, interval };
		IntervalEdge e{ min_max_vals[1], false, interval };
		edge_list.push_back(b);
		edge_list.push_back(e);

		// Expand matrix
		int n = overlap_matrix.size();
		std::vector<int> new_row(n + 1, 0);
		overlap_matrix.push_back(new_row);
		for (size_t i = 0; i < n; i++)
			overlap_matrix[i].push_back(0);

	}

	std::vector<std::shared_ptr<Interval>>& PhysicsManager::getInterval(int axis)
	{
		if (axis == AXIS_X) // x
			return all_x_intervals;
		else if (axis == AXIS_Y) // y
			return all_y_intervals;
		else // assuming z, not safe
			return all_z_intervals;
	}

	void PhysicsManager::addPhysicsObject(std::shared_ptr<PhysicsBody> body) {
		// TODO: std::find? yea.
		bool exists = false;
		for(size_t i = 0; i < all_physics_objects.size(); i++){
			auto &other = all_physics_objects.at(i);
			if (other->parent == body->parent){
				exists = true;
				break;
			}
		}
		if (!exists) {
			all_physics_objects.push_back(body);
			makeInterval(AXIS_X, body);
			makeInterval(AXIS_Y, body);
			makeInterval(AXIS_Z, body);
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

	int PhysicsManager::findIntervalIdx(int axis, std::shared_ptr<Interval> interval) {
		auto& intervals = getInterval(axis);
		for (size_t i = 0; i < intervals.size(); i++) {
			if (intervals[i]->gob_body == interval->gob_body)
				return i;
		}
		return -1;
	}

	std::vector<IntervalEdge>& PhysicsManager::getEdgeList(int axis) {
		std::vector<IntervalEdge> edge_list;
		if (axis == AXIS_X)
			return edge_list_x;
		else if (axis == AXIS_Y)
			return edge_list_y;
		else // assuming z, not safe
			return edge_list_z;
	}

	std::vector<std::vector<int>>& PhysicsManager::getOverlapMatrix(int axis)
	{
		if (axis == 0)
			return overlap_mat_x;
		else if (axis == 1)
			return overlap_mat_y;
		else // assuming z (not safe lol)
			return overlap_mat_z;
	}

	bool& PhysicsManager::getTraverseEdgeListBool(int axis)
	{
		if (axis == 0)
			return traverse_edge_list_x;
		else if (axis == 1)
			return traverse_edge_list_y;
		else // assuming z (not safe lol)
			return traverse_edge_list_z;
	}

	void PhysicsManager::BubbleSort(int axis) {

		auto& edge_list = getEdgeList(axis);

		size_t n = edge_list.size();
		for (size_t i = 0; i < n - 1; i++) {
			for (size_t j = 0; j < n - 1 - i; j++) {
				if (edge_list[j + 1].val < edge_list[j].val) {
					auto &tmp = edge_list[j];
					edge_list[j] = edge_list[j + 1];
					edge_list[j + 1] = tmp;
				}
			}
		}
	}

	void PhysicsManager::TraverseEdgeList(int axis) {

		auto& edge_list = getEdgeList(axis);
		auto& overlap_matrix = getOverlapMatrix(axis);

		//T raverse edge_list from start to end
		for (size_t i = 0; i < edge_list.size(); i++) {

			auto &interval_edge = edge_list[i];

			if (interval_edge.is_b) {
				// When a b is encounted, mark corresponding object interval as active in an active_interval_list
				interval_edge.interval->active = true;
				active_interval_list.push_back(interval_edge.interval);
				if (active_interval_list.size() > 1) {
					// If more than one interval in active_interval_list, then update the overlap_matrix in right position
					for (size_t j = 0; j < active_interval_list.size(); j++) {
						for (size_t k = j; k < active_interval_list.size(); k++) {
							auto interval_a = findIntervalIdx(axis, active_interval_list[j]);
							auto interval_b = findIntervalIdx(axis, active_interval_list[k]);
							auto a = std::min(interval_a, interval_b);
							auto b = std::max(interval_a, interval_b);
							overlap_matrix[a][b] = 0;
						}
					}

				}
			}

			else {

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

	void PhysicsManager::doThatThing(int axis) {
		auto &edge_list = getEdgeList(axis);
		for (size_t i = 0; i < edge_list.size(); i++) {
			auto& interval = edge_list[i].interval;
			auto min_maxes = interval->gob_body->collider->getMinMaxDims();
			auto &min_max = min_maxes[axis];
			auto val = edge_list[i].is_b ? min_max[0] : min_max[1];
			edge_list[i].val = val;
		}
	}

	void PhysicsManager::doThatOtherThing(int axis) {

		bool& traverse_edge_list = getTraverseEdgeListBool(axis);

		auto& edge_list = getEdgeList(axis);
		auto& overlap_matrix = getOverlapMatrix(axis);

		//simple/nice case
		if (!traverse_edge_list) {

			//list is not sorted, interval values are updated
			size_t n = edge_list.size();
			for (size_t i = 0; i < n - 1; i++) {
				for (size_t j = 0; j < n - 1 - i; j++) {
					if (edge_list[j + 1].val < edge_list[j].val) {
						int interval_a = findIntervalIdx(axis, edge_list[j].interval);
						int interval_b = findIntervalIdx(axis, edge_list[j + 1].interval);

						if (interval_a == -1 || interval_b == -1) {
							Logger::log("oh fuck...(inside sweepAndMotherFuckingPrune\n");
							std::exit(1);
						}

						auto a = std::min(interval_a, interval_b);
						auto b = std::max(interval_a, interval_b);

						// Toggle between 0 (false) and 1 (true)
						overlap_matrix[a][b] = overlap_matrix[a][b] == 0 ? 1 : 0;

						auto& tmp = edge_list[j];
						edge_list[j] = edge_list[j + 1];
						edge_list[j + 1] = tmp;
					}
				}
			}

		}
		else {
			BubbleSort(axis);
			TraverseEdgeList(axis);
			traverse_edge_list = false;
		}
	}

	std::vector<CollidingObject> PhysicsManager::sweepAndMotherfuckingPrune() {

		doThatThing(AXIS_X);
		doThatThing(AXIS_Y);
		doThatThing(AXIS_Z);

		doThatOtherThing(AXIS_X);
		doThatOtherThing(AXIS_Y);
		doThatOtherThing(AXIS_Z);

		std::vector<CollidingObject> collisions;

		// Final check!
		size_t rows = overlap_mat_x.size();
		size_t cols = overlap_mat_x[0].size();
		for (size_t i = 0 ; i < rows; i++) {
			for (size_t j = i + 1; j < cols; j++) {
				int overlap_x = overlap_mat_x[i][j];
				int overlap_y = overlap_mat_y[i][j];
				int overlap_z = overlap_mat_z[i][j];
					
				if (overlap_x && overlap_y && overlap_z) {
					CollidingObject col;
					auto &gob_body_a = all_x_intervals[i]->gob_body;
					auto &gob_body_b = all_x_intervals[j]->gob_body;

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
		collision_last_frame = collisions.size();
		collisions.clear();
	}

	std::optional<glm::vec3> PhysicsManager::getImpulseFromCollision(std::shared_ptr <PhysicsBody> a, std::shared_ptr<PhysicsBody> b) {
		
		if (a->parent->to_be_destroyed_ || b->parent->to_be_destroyed_)
			return std::nullopt;

		// Check penetration level (sus)
		auto min_max_a = a->collider->getMinMaxDims();
		auto &min_max_x_a = min_max_a[0];
		auto &min_max_y_a = min_max_a[1];
		auto &min_max_z_a = min_max_a[2];

		auto min_max_b = b->collider->getMinMaxDims();
		auto &min_max_x_b = min_max_b[0];
		auto &min_max_y_b = min_max_b[1];
		auto &min_max_z_b = min_max_b[2];
		
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