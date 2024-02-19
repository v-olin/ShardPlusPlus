#pragma once

#include "SceneManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "GUI.h"

#include "CubeMap.h"

#include <glm.hpp>

namespace Shard {
	class Renderer {
	public:
		Renderer(SceneManager& sceneManager,
			TextureManager& texManager,
			ShaderManager& shaderManager,
			GUI& gui,
			GLFWwindow* window);

		void render();

		float m_nearPlane = 1.f;
		float m_farPlane = 300.f;

	private:
		SceneManager& m_sceneManager;
		TextureManager& m_textureManager;
		ShaderManager& m_shaderManager;
		GUI& m_gui;
		const glm::vec2 m_resolution;
		const float m_fieldOfView;
		glm::mat4 m_projectionMatrix;
		bool m_drawColliders;
		GLFWwindow* m_window;

		GLuint cubemap_tex_id{ 0 };
		CubeMap* cubemap_model{ nullptr };

		GLuint envmap_bg_id{ 0 };
		GLuint envmap_refmap_id{ 0 };
		
		GLuint LoadCubeMap(std::string cubemap_name);
		void drawCubeMap();
		void drawBackground();
		void drawScene();
		void drawModels();
		void drawCollider(std::shared_ptr<GameObject> toDraw);
		void configureDefaultShader();
	};
}