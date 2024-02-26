#pragma once

#include "SceneManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "GUI.h"
#include "FBO.h"

#include "CubeMap.h"
#include "heightfield.h"

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
		HeightField m_heightfield;
		const glm::vec2 m_resolution;
		const float m_fieldOfView;
		glm::mat4 m_projectionMatrix;
		bool m_drawColliders;
		GLFWwindow* m_window;
		FBO m_shadowMapFB;

		GLuint cubemap_tex_id{ 0 };
		CubeMap* cubemap_model{ nullptr };

		const std::vector<std::string> m_requiredShaders{
			"background",
			"collider",
			"cubemap",
			"default",
			"heightfield",
			"shadowMap"
		};

		GLuint envmap_bg_id{ 0 };
		GLuint envmap_refmap_id{ 0 };
		GLuint envmap_irrmap_id{ 0 };
		
		void loadRequiredShaders();
		GLuint LoadCubeMap(std::string cubemap_name);
		void drawCubeMap();
		void drawBackground();
		void drawScene(glm::mat4 viewMatrix, glm::mat4 projMatrix, bool drawingShadowMap = false);
		void drawModels(glm::mat4 viewMatrix, glm::mat4 projMatrix, bool drawingShadowMap = false);
		void drawCollider(std::shared_ptr<GameObject> toDraw);
		void configureDefaultShader();
	};
}