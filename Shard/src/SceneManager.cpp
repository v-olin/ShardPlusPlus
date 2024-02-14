#include "SceneManager.h"
#include "Logger.h"
#include "ShaderManager.h"

#include "GameObjectManager.h"
#include <filesystem>
namespace fs = std::filesystem;

#include "gtx/rotate_vector.hpp"

namespace Shard {

	SceneManager& SceneManager::getInstance() {
		static SceneManager instance;
		return instance;
	}

	glm::mat4 SceneManager::getCameraViewMatrix() {
		return camera.viewMatrix();
	}
}