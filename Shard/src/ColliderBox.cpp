#include "ColliderBox.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include "gtx/component_wise.hpp"

#include <vector>

#define FLOAT_MAX std::numeric_limits<float>::max()
#define FLOAT_MIN std::numeric_limits<float>::min()

namespace Shard {

	ColliderBox::ColliderBox(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Model> model)
		: Collider(game_obj, model)
		, m_boxBottomLeft({ 0.f })
		, m_boxTopRight({ 0.f })
	{ }

	void ColliderBox::recalculateBoundingBox()
	{
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
			{0.0,	0.0,	 0.0},		// v0
			{0.0,	0.0,	 -1},		// v1
			{1,		0.0,	 -1},		// v2
			{1,	    0.0,	 0.0},		// v3
			{0.0,	1,		 0.0},		// v4
			{0.0,	1,		-1},		// v5
			{1,		1,		-1},		// v6
			{1,		1,		0.0}		// v7
		};

		for (auto &vertex : vertices)
			//cant remember if it should be one or zero        here v   , so this might fuck shit up
			vertex = glm::vec3(m_model->getModelMatrix() * glm::vec4(vertex, 1.0));

		auto min = glm::vec3{ FLOAT_MAX };
		auto max = glm::vec3{ FLOAT_MIN };

		for (int i = 0; i < 8;  i++) {
			min = glm::min(min, vertices[i]);
			max = glm::max(max, vertices[i]);
		}

		m_boxBottomLeft = min;
		m_boxTopRight = max;
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
		// Re: rotations may be fucked, very bad!!
		return {
			{ m_boxBottomLeft.x, m_boxTopRight.x },
			{ m_boxBottomLeft.y, m_boxTopRight.y },
			{ m_boxBottomLeft.z, m_boxTopRight.z }
		};
	}
}