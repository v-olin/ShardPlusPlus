#include "WaterField.h"

#include "ShaderManager.h"

#include <glm.hpp>
#include <gtx/transform.hpp>

namespace Shard {
	
	WaterField::WaterField(SceneManager& sceneManager)
		: m_meshResolution(0)
		, m_vao(0)
		, m_pbo(0)
		, m_ubo(0)
		, m_ibo(0)
		, m_numIndices(0)
		, m_tesselation(0)
		, m_sceneManager(sceneManager)
	{ }

	void WaterField::generateMesh(int tesselation, float size) {
		std::vector<glm::vec3> vertices{};
		std::vector<int> indices{};
		std::vector<glm::vec2> uvs{};

		m_tesselation = tesselation;

		float sideLen = size / tesselation;
		float uvLen = 1.0f / tesselation;

		for (int i = 0; i < tesselation; i++)
		{
			float x = -size / 2 + i * sideLen;

			for (int j = 0; j < tesselation; j++)
			{
				float z = -size / 2 + j * sideLen;

				vertices.push_back({ x, 0, z });
				uvs.push_back({ i * uvLen, j * uvLen });

				if (i != tesselation - 1 && j != tesselation - 1)
				{
					indices.push_back(i * tesselation + j);
					indices.push_back(i * tesselation + j + 1);
					indices.push_back((i + 1) * tesselation + j + 1);

					indices.push_back(i * tesselation + j);
					indices.push_back((i + 1) * tesselation + j + 1);
					indices.push_back((i + 1) * tesselation + j);
				}
			}
		}

		m_numIndices = indices.size();

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glGenBuffers(1, &m_pbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_pbo);
		glBufferData(GL_ARRAY_BUFFER,
			vertices.size() * sizeof(glm::vec3),
			vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &m_ubo);
		glBindBuffer(GL_ARRAY_BUFFER, m_ubo);
		glBufferData(GL_ARRAY_BUFFER,
			uvs.size() * sizeof(glm::vec2),
			uvs.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &m_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			indices.size() * sizeof(int),
			indices.data(), GL_STATIC_DRAW);
	}

	void WaterField::submitTriangles(const glm::mat4& viewMatrix,
		const glm::mat4& projMatrix,
		GLuint envMap, GLuint irradMap, GLuint refMap) {

		auto sm = ShaderManager::getInstance();
		auto shader = sm.getShader("waterfield");

		glUseProgram(shader);

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, envMap);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, irradMap);
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, refMap);
		glActiveTexture(GL_TEXTURE0);


		sm.SetMat4x4(shader, projMatrix * viewMatrix * glm::mat4(1.0f), "modelViewProjectionMatrix");

		//labhelper::setUniformSlow(m_shaderProgram, "currentTime",
		//	sceneContext.currentTime);
		sm.SetFloat1(shader, m_sceneManager.currentTime, "currentTime");

		float tWidth = 1.0f / m_tesselation;
		//labhelper::setUniformSlow(m_shaderProgram, "meshWidth", tWidth);

		sm.SetFloat1(shader, tWidth, "meshWidth");

		// uniform mat4 viewInverse;
		//labhelper::setUniformSlow(m_shaderProgram, "viewInverse", inverse(viewMatrix));

		sm.SetMat4x4(shader, glm::inverse(viewMatrix), "viewInverse");

		// uniform vec3 viewSpaceLightPosition;
		//labhelper::setUniformSlow(m_shaderProgram, "viewSpaceLightPosition", vec3(viewSpaceLightPosition));
		glm::vec4 viewSpaceLightPosition =
			viewMatrix * glm::vec4(m_sceneManager.sun.light_position, 1.0f);
		sm.SetVec3(shader, glm::vec3(viewSpaceLightPosition), "viewSpaceLightPosition");

		//uniform mat4 normalMatrix;
		//labhelper::setUniformSlow(m_shaderProgram, "normalMatrix",
		//	inverse(transpose(viewMatrix * mat4(1.0f))));

		sm.SetMat4x4(shader, glm::inverse(glm::transpose(viewMatrix * glm::mat4(1.0f))), "normalMatrix");

		//uniform mat4 modelViewMatrix;
		//labhelper::setUniformSlow(m_shaderProgram, "modelViewMatrix", viewMatrix * mat4(1.0f));

		sm.SetMat4x4(shader, viewMatrix * glm::mat4(1.0f), "modelViewMatrix");

		glBindVertexArray(m_vao);
		glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);

		glUseProgram(0);
	}
}