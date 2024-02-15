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

		glm::vec3 max = m_model->max;
		glm::vec3 min = m_model->min;

		//removed - signs since min can already be negativ if neede, very bad!!
		std::vector<glm::vec3> vertices{
			{min.x,	min.y, min.z},	// v0
			{min.x,	min.y, max.z},	// v1
			{max.x,	min.y, max.z},	// v2
			{max.x,	min.y, min.z},	// v3
			{min.x,	max.y, min.z},	// v4
			{min.x,	max.y, max.z},	// v5
			{max.x,	max.y, max.z},	// v6
			{max.x,	max.y, min.z}	// v7
		};

		//for (auto &vertex : vertices)
		//	//cant remember if it should be one or zero here v, so this might fuck shit up
		//	vertex = glm::vec3(m_model->getModelMatrix() * glm::vec4(vertex, 1.0));

		auto minn = glm::vec3{ FLOAT_MAX };
		auto maxx = glm::vec3{ FLOAT_MIN };

		for (int i = 0; i < 8;  i++) {
			minn = glm::min(minn, vertices[i]);
			maxx = glm::max(maxx, vertices[i]);
		}

		m_boxBottomLeft = minn;
		m_boxTopRight = maxx;
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