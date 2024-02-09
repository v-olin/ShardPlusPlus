#pragma once

#include <memory>
#include <string>
#include <glm.hpp>

namespace Shard {
	class Transform : public std::enable_shared_from_this<Transform> {
	public:
		// this enables use to use GLM for translate, scale & rotation
		// while also encoding position, scaling and rotation
		glm::mat4 transformMatrix{ 1.0f };
		glm::mat4 lastTransformMatrix{ 1.0f }; // lx, ly, lz
		glm::vec3 forward{ 0.0f }, right{ 0.0f }, up{ 0.0f };

		std::string sprite_path{};

		Transform();
		Transform(std::shared_ptr<Transform> src);

		//Returns coords for centre of transform
		glm::vec3 position();
		glm::vec3 rotation();
		glm::vec3 size();
		glm::vec3 scale();
		void translate(const glm::vec3& force);
		void rotate(const float angle_deg, const glm::vec3& axis);
		void scale(const glm::vec3& scale);
		glm::vec3 getLastDirection();
	private:
		glm::mat3 getRotationMatrix();
	};
}