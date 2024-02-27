#pragma once

#include "GameObject.h"
#include "Camera.h"
#include <memory>
#include <vector>

//class Camera;
//class Model;

namespace Shard {

	typedef struct Sun {
		Sun() = default;
		glm::vec3 light_color{ 1.0f, 1.0f, 1.0f };
		glm::vec3 light_position{ 0.0f, 100.0f, 0.0f }; //<-----------------------------------------
		float light_ambient_intensity{ 0.1f };
		float light_diffuse_intensity{ 0.8f };
		float light_specular_intensity{ 1.0f };

		float attenuation_constant{ 1.0f };
		float attenuation_linear{ 0.0f };
		float attenuation_quadratic{ 0.001f };

	} Sun;
	class SceneManager {
	public:
		static SceneManager& getInstance();

		SceneManager(SceneManager const&) = delete;
		SceneManager& operator=(SceneManager const&) = delete;

		//void Draw();
		glm::mat4 getCameraViewMatrix();

		Camera camera{};
		Sun sun{};
	private:
		SceneManager() = default;
	};

}