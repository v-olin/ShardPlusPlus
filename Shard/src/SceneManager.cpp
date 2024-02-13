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
		
		auto base_dir = fs::current_path();
		base_dir = base_dir.parent_path().append("Shard");
		base_dir = 	base_dir.append("res");
		base_dir = base_dir.append("shaders");
		auto copy = base_dir.string();

		copy += "//default.frag";
		base_dir = base_dir.append("default.vert");




		ShaderManager sm{};
		//GLuint shader_id = sm.loadShader(base_dir.string(), copy, false);
	
		GLuint shader_id = sm.loadShader("C:/Chalmers/MPALG1/LP3/Spelmotor/ShardPlusPlus/Shard/res/shaders/default.vert", "C:/Chalmers/MPALG1/LP3/Spelmotor/ShardPlusPlus/Shard/res/shaders/default.frag", false);
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