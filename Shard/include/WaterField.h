#pragma once

#include "SceneManager.h"

#include <glad/glad.h>

namespace Shard {

	class WaterField {
	public:

		WaterField(SceneManager& sceneManager);

		void generateMesh(int tesselation, float size);
		void submitTriangles(const glm::mat4& viewMatrix,
			const glm::mat4& projectionMatrix,
			GLuint envMap, GLuint irradMap, GLuint refMap);

	private:
		int m_meshResolution;
		int m_tesselation;
		GLuint m_vao;
		GLuint m_pbo;
		GLuint m_ibo;
		GLuint m_ubo;
		GLsizei m_numIndices;
		SceneManager& m_sceneManager;
	};

}