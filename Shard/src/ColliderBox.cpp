#include "ColliderBox.h"

#include <glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/component_wise.hpp"

#include <vector>

namespace Shard {

	ColliderBox::ColliderBox(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Transform> transform)
		: Collider(game_obj, transform) 
	{
	}

	void ColliderBox::recalculateBoundingBox()
	{

		//maybe?
		auto size_ =transform->size();
		float size_x = size_.x;
		float size_y = size_.y;
		float size_z = size_.z;

		/*
		    	   v5                    v6
		           *---------------------*
		          /|                    /|
		         / |                   / |
		        /  |                  /  |
		       /   |                 /   |
		      /    |                /    |
	      v4 /     |             v7/     |
		    *---------------------*		 |
		    |      |              |      |
		    |      |              |      |
		    |      |              |      |
		    |      |              |      |
		    |  	   |v1			  |      |
		    |      *--------------|------* v2
		    |     /               |     /
		    |    /                |    /				y    -z
		    |   /                 |   /				   / \	/ \
		    |  /                  |  /					|	/
		    | /                   | /					|  /
		    |/                    |/					| /
		    *---------------------*						|/
		    v0				      v3					*---------> x
		    
		    
		*/

		std::vector<glm::vec3> vertices{
			{0.0,	 0.0,	 0.0},			// v0
			{0.0,	 0.0,	 -size_z},		// v1
			{size_x, 0.0,	 -size_z},		// v2
			{size_x, 0.0,	 0.0},			// v3
			{0.0,	 size_y, 0.0},			// v4
			{0.0,	 size_y, -size_z},		// v5
			{size_x, size_y, -size_z},		// v6
			{size_x, size_y, 0.0}			// v7
		};

		for (auto &vertex : vertices)
			//cant remember if it should be one or zero        here v   , so this might fuck shit up
			vertex = glm::vec3(transform->transformMatrix * glm::vec4(vertex, 1.0));

		auto min = glm::vec3(1.0f) * std::numeric_limits<float>::max();
		auto max = glm::vec3(1.0f) * std::numeric_limits<float>::min();
		for (int i = 0; i < 8;  i++) {
			min = glm::min(min, vertices[i]);
			max = glm::max(max, vertices[i]);
		}

		box_bottom_left = min;
		box_top_right = max;

	}

	std::optional<glm::vec3> ColliderBox::checkCollision(Ray& ray)
	{
		// TODO: Box-ray collision
		return std::nullopt;
	}

	std::vector<glm::vec2> ColliderBox::getMinMaxDims() {
		// TODO: If we notice weird box sizes when e.g. rotating around origin
		// check this stuff because box_bottom_left becomes box_bottom_right
		// and vice versa. U got this, right?
		return {
			{box_bottom_left.x, box_top_right.x},
			{box_bottom_left.y, box_top_right.y},
			{box_bottom_left.z, box_top_right.z}
		};
	}

	void ColliderBox::draw(SDL_Color color) {

	}

}