#pragma once

#include "Collider.h"

namespace Shard {

	class ColliderBox : public Collider , public std::enable_shared_from_this<ColliderBox>{
	public:
        glm::vec3 m_boxBottomLeft;
		glm::vec3 m_boxTopRight;

		ColliderBox(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Model> model);

		void recalculateBoundingBox() override;
		std::optional<glm::vec3> checkCollision(Ray& ray) override;
		std::vector<glm::vec2> getMinMaxDims() override;
		// TODO: renderer should draw box, not the box itself, very bad!!
		//void draw(glm::vec3 color) override;
	};
}