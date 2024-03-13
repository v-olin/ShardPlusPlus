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
	{ 
	}

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

		std::vector<glm::vec3> vertices {
			{min.x,	min.y, max.z},	// v0
			{min.x,	min.y, min.z},	// v1
			{max.x,	min.y, min.z},	// v2
			{max.x,	min.y, max.z},	// v3
			{min.x,	max.y, max.z},	// v4
			{min.x,	max.y, min.z},	// v5
			{max.x,	max.y, min.z},	// v6
			{max.x,	max.y, max.z}	// v7
		};

		for (auto &vertex : vertices)
			vertex = glm::vec3(m_model->getModelMatrix() * glm::vec4(vertex, 1.0));

		auto min_trans = glm::vec3{ 9999999.0f,  9999999.0f,  9999999.0f };
		auto max_trans = -min_trans;

		for (int i = 0; i < 8;  i++) {
			min_trans = glm::min(min_trans, vertices[i]);
			max_trans = glm::max(max_trans, vertices[i]);
		}

		m_transformed_boxBottomLeft = min_trans;
		m_transformed_boxTopRight = max_trans;

		auto minn = min_trans;
		auto maxx = max_trans;

		m_boxBottomLeft = glm::inverse(m_model->getModelMatrix()) * glm::vec4(minn, 1.0);
		m_boxTopRight = glm::inverse(m_model->getModelMatrix()) * glm::vec4(maxx, 1.0);

	}

	std::optional<glm::vec3> ColliderBox::checkCollision(Ray& ray)
	{
		// impl from: https://tavianator.com/2011/ray_box.html
		auto min = m_transformed_boxBottomLeft;
		auto max = m_transformed_boxTopRight;

		double tmin = -99999999.0f, tmax = 99999999.0f;

		if (ray.dir.x != 0.0) {
			double tx1 = (min.x - ray.origin.x)/ray.dir.x;
			double tx2 = (max.x - ray.origin.x)/ray.dir.x;

			tmin = std::max(tmin, std::min(tx1, tx2));
			tmax = std::min(tmax, std::max(tx1, tx2));
		}

		if (ray.dir.y != 0.0) {
			double ty1 = (min.y - ray.origin.y)/ray.dir.y;
			double ty2 = (max.y - ray.origin.y)/ray.dir.y;

			tmin = std::max(tmin, std::min(ty1, ty2));
			tmax = std::min(tmax, std::max(ty1, ty2));
		}
		if (ray.dir.z != 0.0) {
			double tz1 = (min.z - ray.origin.z)/ray.dir.z;
			double tz2 = (max.z - ray.origin.z)/ray.dir.z;

			tmin = std::max(tmin, std::min(tz1, tz2));
			tmax = std::min(tmax, std::max(tz1, tz2));
		}
	
		if (tmax < tmin)
			return std::nullopt;

		auto len = tmin >= 0 ? tmin : tmax;
		if (len < 0)
			return std::nullopt;
		return ray.origin + ray.dir * (float)len;


	}

	std::vector<glm::vec2> ColliderBox::getMinMaxDims() {
		return {
			{ m_boxBottomLeft.x, m_boxTopRight.x },
			{ m_boxBottomLeft.y, m_boxTopRight.y },
			{ m_boxBottomLeft.z, m_boxTopRight.z }
		};
	}
	std::vector<glm::vec2> ColliderBox::getTransformedMinMaxDims() {
		return {
			{ m_transformed_boxBottomLeft.x, m_transformed_boxTopRight.x },
			{ m_transformed_boxBottomLeft.y, m_transformed_boxTopRight.y },
			{ m_transformed_boxBottomLeft.z, m_transformed_boxTopRight.z }
		};
	}
}