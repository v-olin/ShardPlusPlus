#pragma once

#include "SceneManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"

#include "CubeMap.h"

#include <glm.hpp>

namespace Shard {
	class Renderer {
	public:
		Renderer(SceneManager& sceneManager,
			TextureManager& texManager,
			ShaderManager& shaderManager,
			GLFWwindow* window);

		void render();

	private:
		SceneManager& m_sceneManager;
		TextureManager& m_textureManager;
		ShaderManager& m_shaderManager;
		const glm::vec2 m_resolution;
		const float m_fieldOfView;
		const glm::mat4 m_projectionMatrix;
		bool m_drawColliders;
		GLFWwindow* m_window;

		GLuint cubemap_tex_id{ 0 };
		CubeMap* cubemap_model{ nullptr };
		
		GLuint LoadCubeMap(std::string cubemap_name);
		void drawCubeMap();
		void drawScene();
		void drawModels();
		void drawCollider(std::shared_ptr<GameObject> toDraw);
		void configureDefaultShader();
	};
}