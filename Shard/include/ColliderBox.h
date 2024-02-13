#pragma once

#include "Collider.h"

namespace Shard {

	class ColliderBox : public Collider , public std::enable_shared_from_this<ColliderBox>{
	public:
        glm::vec3 box_bottom_left{ 0.0f };
		glm::vec3 box_top_right{ 0.0f };

		ColliderBox(std::shared_ptr<CollisionHandler> game_obj, std::shared_ptr<Transform> transform);

		void recalculateBoundingBox() override;
		std::optional<glm::vec3> checkCollision(Ray& ray) override;
		std::vector<glm::vec2> getMinMaxDims() override;
		void draw(glm::vec3 color) override;

	private:
	};

}