#pragma once

#include "SceneManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "GUI.h"
#include "RenderableObject.h"
#include "CubeMap.h"
#include "heightfield.h"
#include "WaterField.h"

#include <glm.hpp>

namespace Shard {
	class Renderer {
	public:
		Renderer();

		void initialize(SceneManager& sceneManager,
			TextureManager& texManager,
			ShaderManager& shaderManager,
			GUI* gui,
			GLFWwindow* window);
		void addRenderObject(std::shared_ptr<RenderableObject> robj);
		void render();
		
		float m_nearPlane = 1.f;
		float m_farPlane = 300.f;
		bool m_usePathTracing = true;

	private:
		SceneManager* m_sceneManager;
		TextureManager* m_textureManager;
		ShaderManager* m_shaderManager;
		GUI* m_gui;
		HeightField* m_heightfield;
		WaterField* m_waterfield;
		glm::vec2 m_resolution;
		float m_fieldOfView;
		glm::mat4 m_projectionMatrix;
		bool m_drawColliders;
		GLFWwindow* m_window;
		int m_windowHeight;
		int m_windowWidth;

		uint32_t m_pathtracerTxtId;

		GLuint cubemap_tex_id{ 0 };
		CubeMap* cubemap_model{ nullptr };

		const std::vector<std::string> m_requiredShaders{
			"background",
			"collider",
			"cubemap",
			"default",
			"heightfield",
			"waterfield",
			"copyTexture",
			"simple"
		};

		GLuint envmap_bg_id{ 0 };
		GLuint envmap_refmap_id{ 0 };
		GLuint envmap_irrmap_id{ 0 };

		std::vector<std::shared_ptr<RenderableObject>> m_renderObjects;
		
		void loadRequiredShaders();
		GLuint LoadCubeMap(std::string cubemap_name);
		void drawCubeMap();
		void drawBackground();
		void drawScene();
		void drawModels();
		void m_drawCollider(std::shared_ptr<GameObject> toDraw);
		void configureDefaultShader();

		void drawPathTracedScene();
	};
}