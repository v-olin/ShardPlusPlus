#include "SceneManager.h"
#include "Logger.h"
#include "ShaderManager.h"

#include "GameObjectManager.h"
#include <filesystem>
namespace fs = std::filesystem;
namespace Shard {

	void SceneManager::Draw() {
		auto P = glm::perspective(45.0f, (float)1280/(float)760, 1.0f, 100.0f);
		auto V = camera.viewMatrix();
		auto PV = P * V;
		auto PVM = PV;
		
		ShaderManager sm{};
		GLuint shader_id = sm.loadShader("default", false);
		sm.current_shader_id = shader_id;

		glUseProgram(shader_id);

		auto& gobs = GameObjectManager::getInstance().getObjects();
		for (auto &gob : gobs) {
			// Set uniforms
			auto M = gob->body_->trans->transformMatrix;
			PVM = P * V * M;
			sm.SetMat4x4(PVM, "u_MVP");
			// Draw
			gob->model->Draw();
		}
		
		glUseProgram(0);

	}


}