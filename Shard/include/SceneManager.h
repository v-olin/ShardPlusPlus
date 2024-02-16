#pragma once

#include "GameObject.h"
#include "Camera.h"
#include <memory>
#include <vector>

//class Camera;
//class Model;

namespace Shard {

	class SceneManager {
	public:
		static SceneManager& getInstance();

		SceneManager(SceneManager const&) = delete;
		SceneManager& operator=(SceneManager const&) = delete;

		//void Draw();
		glm::mat4 getCameraViewMatrix();

		Camera camera{};
	private:
		SceneManager() = default;
	};

}