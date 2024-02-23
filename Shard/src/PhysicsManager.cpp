#include "PhysicsManager.h"
#include "CollisionHandler.h"
#include "Bootstrap.h"
#include "Logger.h"
#include "ColliderBox.h"
#include "SceneManager.h"

#include <algorithm>
#include <list>
#include <queue>
#include <optional>
#include <unordered_map>
#include <functional>
#include <unordered_map>
#include <functional>

#include "gtx/transform.hpp"

namespace Shard {

	PhysicsManager::PhysicsManager() { }

	PhysicsManager& PhysicsManager::getInstance() {
		static PhysicsManager instance;
		return instance;
	}

	void PhysicsManager::makeInterval(int axis, std::shared_ptr<PhysicsBody> gob_body) {

		// Check penetration level (sus)
		//auto min_max = gob_body->m_collider->getMinMaxDims();
		auto min_max = gob_body->m_collider->getTransformedMinMaxDims();

		//	vertex = glm::vec3(m_model->getModelMatrix() * glm::vec4(vertex, 1.0));
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
		interval->idx = intervals.size() - 1;
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
		// TODO: std::find? yea.
		bool exists = false;
		for (size_t i = 0; i < all_physics_objects.size(); i++) {
			auto& other = all_physics_objects.at(i);
			if (other->m_parent == body->m_parent) {
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
		// TODO: remove body from all intervals, make sure to remove all edges from all edge_lists also
		//and set traverse_edge_list = true for all axies
		//find edges

		auto delete_from_edge_list = [this, body](int axis) {
			auto &edge_list = this->getEdgeList(axis);
			edge_list.erase(std::remove_if(edge_list.begin(), edge_list.end(), [&](const IntervalEdge ie) -> bool {
				return ie.interval->gob_body == body;
			}), edge_list.end());
		};

		delete_from_edge_list(AXIS_X);
		delete_from_edge_list(AXIS_Y);
		delete_from_edge_list(AXIS_Z);

		auto delete_from_interval_list = [this, body](int axis) {
			auto &interval_list = getInterval(axis);
			interval_list.erase(std::remove_if(interval_list.begin(), interval_list.end(), [&](const std::shared_ptr<Interval>& i) -> bool {
				return i->gob_body == body;
				}), interval_list.end());
			};

		delete_from_interval_list(AXIS_X);
		delete_from_interval_list(AXIS_Y);
		delete_from_interval_list(AXIS_Z);

		auto shrink_overlap_mat = [this, body](int axis) {
			auto& mat = getOverlapMatrix(axis);
			auto len = mat.size();
			for (size_t i = 0; i < mat.size(); i++) {
				mat[i].erase(std::next(mat[i].begin(), len - 1));
			}
			mat.erase(std::next(mat.begin(), len - 1));
			};
		
		shrink_overlap_mat(AXIS_X);
		shrink_overlap_mat(AXIS_Y);
		shrink_overlap_mat(AXIS_Z);

		//remove from activeintervalks list
		active_interval_list.erase(std::remove_if(active_interval_list.begin(), active_interval_list.end(), [&](const std::shared_ptr<Interval>& i) -> bool {
			return i->gob_body == body;
			}), active_interval_list.end());


		for(size_t i = 0; i < all_physics_objects.size(); i++){
			auto other = all_physics_objects.at(i);
			if(other->m_parent == body->m_parent){
				auto iter = all_physics_objects.begin();
				std::advance(iter, i);
				all_physics_objects.erase(iter);
			}
		}

		resetIdx(AXIS_X);
		resetIdx(AXIS_Y);
		resetIdx(AXIS_Z);

		traverse_edge_list_x = true;
		traverse_edge_list_y = true;
		traverse_edge_list_z = true;
	}
	void PhysicsManager::resetIdx(int axis) {
		auto interval_list = getInterval(axis);
		for (size_t i = 0; i < interval_list.size(); i++)
			interval_list[i]->idx = i;
	}

	bool PhysicsManager::willTick() {
		// TODO: remove when not testing
		//return true;
		return Bootstrap::getCurrentMillis() - last_update > time_interval;
	}

	bool PhysicsManager::update() {
		std::list<size_t> to_remove;

		if (!willTick())
			return false;

		last_update = Bootstrap::getCurrentMillis();

		for (auto &body : all_physics_objects) {
			if (body->m_usesGravity)
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
			auto a_handler = std::dynamic_pointer_cast<CollisionHandler>(col.a->m_parent);
			auto b_handler = std::dynamic_pointer_cast<CollisionHandler>(col.b->m_parent);

			if (!a_handler || !b_handler) {
				Logger::log("failed upcast", LOG_LEVEL_ALL);
			}

			if (col.a->m_parent->m_toBeDestroyed) {
				if (!col.b->m_parent->m_toBeDestroyed)
					b_handler->onCollisionExit(nullptr);
				to_remove.push_back(i++);
				continue;
			}

			if (col.b->m_parent->m_toBeDestroyed) {
				if (!col.a->m_parent->m_toBeDestroyed)
					a_handler->onCollisionExit(nullptr);
				to_remove.push_back(i++);
				continue;
			}

			
			std::optional<glm::vec3> impulse = getImpulseFromCollision(col.a, col.b);
			//if we have a collision, run the impulse stuff

			if (impulse.has_value()) {
				runCollisionReaction(impulse.value(), col);
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
		if (removed > 0) {
			traverse_edge_list_x = true;
			traverse_edge_list_y = true;
			traverse_edge_list_z = true;


		}

		// TODO: don't run if no game objects?
		if (all_physics_objects.size() > 1)
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

	//TODO, remove this and move ids into an intervall, and set ids in add/remove object
	/*int PhysicsManager::findIntervalIdx(int axis, std::shared_ptr<Interval> interval) {
		auto& intervals = getInterval(axis);
		for (size_t i = 0; i < intervals.size(); i++) {
			if (intervals[i]->gob_body == interval->gob_body)
				return i;
		}
		return -1;
	}*/

	std::vector<IntervalEdge>& PhysicsManager::getEdgeList(int axis) {
		if (axis == AXIS_X)
			return edge_list_x;
		else if (axis == AXIS_Y)
			return edge_list_y;
		else if (axis == AXIS_Z)
			return edge_list_z;
		else
			assert(false);
	}

	std::vector<std::vector<int>>& PhysicsManager::getOverlapMatrix(int axis)
	{
		if (axis == AXIS_X)
			return overlap_mat_x;
		else if (axis == AXIS_Y)
			return overlap_mat_y;
		else if (axis == AXIS_Z)
			return overlap_mat_z;
		else
			assert(false);
	}

	bool& PhysicsManager::getTraverseEdgeListBool(int axis)
	{
		if (axis == AXIS_X)
			return traverse_edge_list_x;
		else if (axis == AXIS_Y)
			return traverse_edge_list_y;
		else if (axis == AXIS_Z)
			return traverse_edge_list_z;
		else
			assert(false);
	}

	void PhysicsManager::BubbleSort(int axis) {
		auto& edge_list = getEdgeList(axis);
		size_t n = edge_list.size();
		for (size_t i = 0; i < n - 1; i++) {
			for (size_t j = 0; j < n - 1 - i; j++) {
				if (edge_list[j + 1].val < edge_list[j].val) {
					//this was not copy, maybe very bad!
					auto tmp = edge_list[j];
					edge_list[j] = edge_list[j + 1];
					edge_list[j + 1] = tmp;
				}
			}
		}
	}

	void PhysicsManager::TraverseEdgeList(int axis) {

		auto& edge_list = getEdgeList(axis);
		auto& overlap_matrix = getOverlapMatrix(axis);

		for (size_t i = 0; i < overlap_matrix.size(); i++) {
			for (size_t j = 0; j < overlap_matrix.size(); j++) {
				overlap_matrix[i][j] = 0;
			}
		}

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
							auto interval_a = active_interval_list[j]->idx; //findIntervalIdx(axis, active_interval_list[j]);
							auto interval_b = active_interval_list[k]->idx;//findIntervalIdx(axis, active_interval_list[k]);
							auto a = std::min(interval_a, interval_b);
							auto b = std::max(interval_a, interval_b);
							overlap_matrix[a][b] = 1; // TODO: should dis be 1??
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

	//update edge list for given axi
	void PhysicsManager::updateEdgeList(int axis) {
		auto &edge_list = getEdgeList(axis);
		for (size_t i = 0; i < edge_list.size(); i++) {
			auto& interval = edge_list[i].interval;
			auto min_maxes = interval->gob_body->m_collider->getTransformedMinMaxDims();
			auto min_max = min_maxes[axis];
			auto val = edge_list[i].is_b ? min_max[0] : min_max[1];
			edge_list[i].val = val;
		}
	}

	void PhysicsManager::findOverlaps(int axis) {

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

						if (edge_list[j + 1].is_b != edge_list[j].is_b) {
							int interval_a = edge_list[j].interval->idx; //findIntervalIdx(axis, edge_list[j].interval);
							int interval_b = edge_list[j+1].interval->idx;//findIntervalIdx(axis, edge_list[j + 1].interval);

							if (interval_a == -1 || interval_b == -1) {
								Logger::log("oh fuck...(inside sweepAndMotherFuckingPrune\n");
								std::exit(1);
							}

							auto a = std::min(interval_a, interval_b);
							auto b = std::max(interval_a, interval_b);

							// Toggle between 0 (false) and 1 (true)
							overlap_matrix[a][b] = overlap_matrix[a][b] == 0 ? 1 : 0;
							//overlap_matrix[b][a] = overlap_matrix[b][a] == 0 ? 1 : 0;
						}

						//another unlucky non-copy
						auto tmp = edge_list[j];
						edge_list[j] = edge_list[j + 1];
						edge_list[j + 1] = tmp;
					}
				}
			}
		} else {
			BubbleSort(axis);
			TraverseEdgeList(axis);
			traverse_edge_list = false;
		}
	}

	// TODO: return reference
	std::vector<CollidingObject> PhysicsManager::sweepAndMotherfuckingPrune() {

		updateEdgeList(AXIS_X);
		updateEdgeList(AXIS_Y);
		updateEdgeList(AXIS_Z);

		findOverlaps(AXIS_X);
		findOverlaps(AXIS_Y);
		findOverlaps(AXIS_Z);

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

					if (gob_body_a->m_mass > gob_body_b->m_mass) {
						col.a = gob_body_a;
						col.b = gob_body_b;
					}
					else {
						col.a = gob_body_b;
						col.b = gob_body_a;
					}
					if(!findColliding(col.a, col.b))
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


		// TODO:
		/*
			Add bob-box collision checking with non world-space axis aligned boxes
			e.g. use SAT or transform one box into the others realm and do standard AABB there, whatever floats your boat bro
		*/
		auto cols_to_check = sweepAndMotherfuckingPrune();
		for (CollidingObject &col_obj : cols_to_check) {
			possible_impulse = getImpulseFromCollision(col_obj.a, col_obj.b);

			if (possible_impulse.has_value()) {
				impulse = possible_impulse.value();
				runCollisionReaction(impulse, col_obj);

				(std::dynamic_pointer_cast<CollisionHandler>(col_obj.a->m_parent))->onCollisionEnter(col_obj.b);
				(std::dynamic_pointer_cast<CollisionHandler>(col_obj.b->m_parent))->onCollisionEnter(col_obj.a);

				if(!findColliding(col_obj.a->m_parent->m_body, col_obj.b->m_parent->m_body))
					collisions.push_back(col_obj);
			}
		}
	}

	void PhysicsManager::checkForCollisions() {
		collision_last_frame = collisions.size();
		//collisions.clear();
		runCollisionCheck();
		collision_last_frame = collisions.size();
	}

	std::optional<glm::vec3> PhysicsManager::getImpulseFromCollision(std::shared_ptr <PhysicsBody> a, std::shared_ptr<PhysicsBody> b) {
		
		if (a->m_parent->m_toBeDestroyed || b->m_parent->m_toBeDestroyed)
			return std::nullopt;

		// Check penetration level (sus)
		const auto min_max_a = a->m_collider->getTransformedMinMaxDims();
		const auto &min_max_x_a = min_max_a[0];
		const auto &min_max_y_a = min_max_a[1];
		const auto &min_max_z_a = min_max_a[2];
		
		const auto min_max_b = b->m_collider->getTransformedMinMaxDims();
		const auto &min_max_x_b = min_max_b[0];
		const auto &min_max_y_b = min_max_b[1];
		const auto &min_max_z_b = min_max_b[2];
		
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

		static auto calculateOverlapDistance = [](const glm::vec2& a, const glm::vec2& b) {
			float a_start = a[0], a_end = a[1];
			float b_start = b[0], b_end = b[1];

			if (a_end < b_start || b_end < a_start)
				return 0.0f;

			float overlap_start = std::max(a_start, b_start);
			float overlap_end = std::min(a_end, b_end);
			float overlap_distance = overlap_end - overlap_start;
			//check if it should be negative
			if (a_start < b_start)
				overlap_distance *= -1;

			return overlap_distance;
		};

		const float overlap_x = calculateOverlapDistance(min_max_x_a, min_max_x_b);
		const float overlap_y = calculateOverlapDistance(min_max_y_a, min_max_y_b);
		const float overlap_z = calculateOverlapDistance(min_max_z_a, min_max_z_b);

		// Only interested in axis with shortest overlap
		auto abs_over_x = abs(overlap_x);
		auto abs_over_y = abs(overlap_y);
		auto abs_over_z = abs(overlap_z);
		auto min_overlap = std::min({ abs_over_x, abs_over_y, abs_over_z });

		//branchless programming <3
		auto x = abs_over_x == min_overlap ? overlap_x : 0;
		auto y = abs_over_y == min_overlap ? overlap_y : 0;
		auto z = abs_over_z == min_overlap ? overlap_z : 0;
	

		if (overlap_x == 0 || overlap_y == 0 || overlap_z == 0)
			return std::nullopt;

		auto v = glm::vec3{ x, y, z };
		return std::optional<glm::vec3>(v + v * glm::vec3(15.0f));

		/*
		if (overlap_x == min_overlap)
			return std::optional<glm::vec3>({ overlap_x, 0.0f, 0.0f });

		else if (overlap_y == min_overlap)
			return std::optional<glm::vec3>({ 0.0f, overlap_y, 0.0f });

		else if (overlap_z == min_overlap)
			return std::optional<glm::vec3>({ 0.0f, 0.0f, overlap_z });
		*/
		
		// uuuuuuuuuuuuuuhhhh.............. no penetration... but collided?
		// 100 megawhat?

	}

	void PhysicsManager::runCollisionReaction(glm::vec3 impulse, CollidingObject col_obj) {

		float mass_total, mass_prop = 0.f;
		if (!col_obj.a->m_passThrough && !col_obj.b->m_passThrough) {
			mass_total = col_obj.a->m_mass + col_obj.b->m_mass;
		
			if (col_obj.a->m_isKinematic)
				mass_prop = 1.f;
			else
				mass_prop = col_obj.a->m_mass / mass_total;
		
			if (col_obj.a->m_impartForce) {
				col_obj.a->impartForces(col_obj.b, mass_prop);
				col_obj.a->reduceForces(1.f - mass_prop);
			}
		
			if (!col_obj.b->m_isKinematic) {
				glm::vec3 force{ -impulse.x, -impulse.y, -impulse.z};
				col_obj.b->m_parent->m_model->translate(force * mass_prop);
				mass_prop = 1.f - mass_prop;
			}
			else {
				mass_prop = 1.f;
			}
		
			if (!col_obj.a->m_isKinematic) {
				glm::vec3 force{ impulse.x, impulse.y, impulse.z};
				col_obj.a->m_parent->m_model->translate(force * mass_prop);
			}
			
			if (col_obj.a->m_stopOnCollision)
				col_obj.a->stopForces();
		
			if (col_obj.b->m_stopOnCollision)
				col_obj.b->stopForces();
		
		
		
			if (col_obj.a->m_reflectOnCollision)
				col_obj.a->reflectForces(impulse);
			if (col_obj.b->m_reflectOnCollision)
				col_obj.b->reflectForces(impulse);
		}
	}


	//Klick -> collision with body

	std::optional<glm::vec3> PhysicsManager::clickHitsBody(InputEvent ie, std::shared_ptr<PhysicsBody> body) {
		auto ray = getRayFromClick(ie);
		return body->checkCollision(ray);
	}

	Ray PhysicsManager::getRayFromClick(InputEvent ie) {

		auto mouseX = ie.x;
		auto mouseY = ie.y;
		
		// TODO: Don't hardcode
		int windowWidth = 1280;
		int windowHeight = 760;
		
		// Normalize mouse coordinates
		float normalizedX = (2.0f * mouseX) / windowWidth - 1.0f;
		float normalizedY = 1.0f - (2.0f * mouseY) / windowHeight;

		// Create near and far points in view space
		glm::vec4 nearPoint = glm::vec4(normalizedX, normalizedY, 0.0f, 1.0f);
		glm::vec4 farPoint = glm::vec4(normalizedX, normalizedY, 1.0f, 1.0f);

		// Invert the projection matrix to get view-to-world transformation
		// PV^(-1)
		auto P = glm::perspective(45.0f, 1280.0f / 760.0f, 1.f, 300.f);
		auto V = SceneManager::getInstance().getCameraViewMatrix();
		glm::mat4 invProjectionMatrix = glm::inverse(P*V);

		// Unproject points to world space
		glm::vec4 nearPointWorld = invProjectionMatrix * nearPoint;
		glm::vec4 farPointWorld = invProjectionMatrix * farPoint;

		// Divide by w component to get homogeneous coordinates
		nearPointWorld /= nearPointWorld.w;
		farPointWorld /= farPointWorld.w;

		// Create ray direction
		glm::vec3 rayDirection = glm::normalize(glm::vec3(farPointWorld - nearPointWorld));

		// Create and return the ray
		return Ray{ glm::vec3(nearPointWorld), rayDirection };

	}


}