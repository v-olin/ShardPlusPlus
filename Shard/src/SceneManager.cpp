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

	/*
	void SceneManager::Draw() {
		auto P = glm::perspective(20.0f, (float)1280/(float)760, 1.0f, 100.0f);
		auto V = camera.viewMatrix();
		auto PV = P * V;
		auto PVM = PV;
		
		ShaderManager sm{};
		GLuint shader_id = sm.loadShader("default", false);
		sm.current_shader_id = shader_id;

		glUseProgram(shader_id);

		auto& gobs = GameObjectManager::getInstance().getObjects();
		for (auto& gob : gobs) {
			// Set uniforms
			auto M = gob->body_->trans->transformMatrix;

			// :)
			camera.pos = gob->body_->trans->position() + glm::vec3{ 0.0f, 20.0f, 0.0f } + gob->body_->trans->forward * 20.0f;
			camera.front = -gob->body_->trans->forward;
			glm::mat4 mat = glm::rotate(glm::radians(20.0f), glm::vec3{0.0f, 0.0f, 1.0f});
			auto mat2 = mat * glm::vec4(camera.front, 1.0f);
			camera.front = glm::vec3(mat2);
			camera.up = glm::vec3(mat * glm::vec4{ 0.0f, 1.0f, 0.0f, 0.0f });
			camera.right = glm::normalize(glm::cross(camera.front, camera.up));

			PVM = P * camera.viewMatrix() * M;

			sm.SetMat4x4(PVM, "u_MVP");
			// Draw
			gob->model->Draw();
		}
		
		glUseProgram(0);
	}
	*/

	glm::mat4 SceneManager::getCameraViewMatrix() {
		return camera.viewMatrix();
	}


}