#pragma once

#include "SceneManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "GUI.h"

#include "CubeMap.h"
#include "heightfield.h"

#include <glm.hpp>

namespace Shard {
	class Renderer {
	public:
		Renderer(SceneManager& sceneManager,
			TextureManager& texManager,
			ShaderManager& shaderManager,
			GUI* gui,
			GLFWwindow* window);

		void render();
		
		float m_nearPlane = 1.f;
		float m_farPlane = 300.f;
		bool m_usePathTracing = true;

	private:
		SceneManager& m_sceneManager;
		TextureManager& m_textureManager;
		ShaderManager& m_shaderManager;
		GUI* m_gui;
		HeightField m_heightfield;
		const glm::vec2 m_resolution;
		const float m_fieldOfView;
		glm::mat4 m_projectionMatrix;
		bool m_drawColliders;
		GLFWwindow* m_window;
		int m_windowHeight;
		int m_windowWidth;

		uint32_t m_pathtracerTxtId;

		GLuint cubemap_tex_id{ 0 };
		CubeMap* cubemap_model{ nullptr };

		GLuint att_vao{ 0 };
		GLuint att_pbo{ 0 };
		GLuint att_tbo{ 0 };
		GLuint att_ibo{ 0 };

		GLuint att_overlay{ 0 };
		GLuint att_background{ 0 };
		GLuint att_transmap{ 0 };
		std::shared_ptr<Model> m_plane{ nullptr };

		GLuint createGauges();
		void drawGauges();
		void getPlaneAngles(float* pitch, float* roll);

		const std::vector<std::string> m_requiredShaders{
			"background",
			"collider",
			"cubemap",
			"default",
			"heightfield",
			"copyTexture",
			"gauge"
		};

		GLuint envmap_bg_id{ 0 };
		GLuint envmap_refmap_id{ 0 };
		GLuint envmap_irrmap_id{ 0 };
		
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