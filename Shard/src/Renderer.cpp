#include "Renderer.h"

#include "GameObjectManager.h"
#include "Logger.h"

namespace Shard {

	Renderer::Renderer(SceneManager& sceneManager, TextureManager& texManager, ShaderManager& shaderManager, GLFWwindow* window)
		: m_sceneManager(sceneManager)
		, m_textureManager(texManager)
		, m_shaderManager(shaderManager)
		, m_resolution({ 1280, 760 })
		, m_fieldOfView(45.f)
		, m_projectionMatrix(glm::perspective(m_fieldOfView, m_resolution.x / m_resolution.y, 1.f, 300.f))
		, m_drawColliders(true)
		, m_window(window)
	{
		static bool initialized = false;
		if (initialized) {
			Logger::log("Renderer already initialized, very bad!!", LOG_LEVEL_FATAL);
		}
		initialized = true;
	}

	void Renderer::render() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, (int)(m_resolution.x), (int)(m_resolution.y));

		// i have no fucking idea what this does (???)
		// stolen from old codebase
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawScene();

		// should check for errors here
		// surely there are no errors
		// very bad!!

		glfwSwapBuffers(m_window);
	}

	void Renderer::drawScene() {
		glm::mat4 viewMatrix = m_sceneManager.getCameraViewMatrix();

		// here we can draw a background or smth

		drawModels();

		// we can also draw sun or smth
	}

	void Renderer::drawModels() {
		// TODO: this is fucked, they should come from sceneManager
		// but because models are not transforms this is fucked
		// physicsbodies should not have transforms, they should
		// be based on the model
		// TLDR: this is fucked
		auto& gobs = GameObjectManager::getInstance().getObjects();

		for (std::shared_ptr<GameObject> gob : gobs) {
			if (!gob->m_model->m_hasDedicatedShader) [[likely]] {
				configureDefaultShader();
			}

			// this is fucked up
			// glm::mat4 modelMatrix = gob->body_->trans->transformMatrix;
			// much better :^)
			glm::mat4 modelMatrix = gob->m_model->getModelMatrix();
			glm::mat4 viewMatrix = m_sceneManager.getCameraViewMatrix();
			glm::mat4 mvpMatrix = m_projectionMatrix * viewMatrix * modelMatrix;

			const GLuint defShader = m_shaderManager.getDefaultShader();
			if (!gob->m_model->m_hasDedicatedShader) [[likely]] {
				m_shaderManager.SetMat4x4(defShader, mvpMatrix, "u_MVP");
			}

			gob->m_model->Draw();

			if (true) { // if debug
				drawCollider(gob);
			}
		}

		glUseProgram(0);
	}

	void Renderer::drawCollider(std::shared_ptr<GameObject> toDraw) {

		std::vector<glm::vec2> minMax = toDraw->m_body->m_collider->getMinMaxDims();

		glm::vec3 max = glm::vec3{
			minMax[0].y,
			minMax[1].y,
			minMax[2].y
		};

		glm::vec3 min = glm::vec3{
			minMax[0].x,
			minMax[1].x,
			minMax[2].x
		};

		//removed - signs since min can already be negativ if neede, very bad!!
		float vertices[] = {
			min.x,	min.y, min.z,	// v0
			min.x,	min.y, max.z,	// v1
			max.x,	min.y, max.z,	// v2
			max.x,	min.y, min.z,	// v3
			min.x,	max.y, min.z,	// v4
			min.x,	max.y, max.z,	// v5
			max.x,	max.y, max.z,	// v6
			max.x,	max.y, min.z	// v7
		};

		GLuint indices[] = {
			0, 2, 3,
			0, 1, 2,
			0, 1, 4,
			1, 5, 4,
			4, 5, 7,
			6, 5, 7,
			3, 2, 7,
			6, 7, 2,
			0, 3, 4,
			4, 7, 3,
			1, 6, 2,
			1, 5, 6
		};

		glm::mat4 modelMatrix = toDraw->m_model->getModelMatrix();
		glm::mat4 viewMatrix = m_sceneManager.getCameraViewMatrix();
		glm::mat4 mvpMatrix = m_projectionMatrix * viewMatrix * modelMatrix;

		auto shader = m_shaderManager.getShader("collider");
		glUseProgram(shader);

		m_shaderManager.SetVec3(shader, toDraw->m_body->m_debugColor, "colorIn");
		m_shaderManager.SetMat4x4(shader, mvpMatrix, "u_MVP");

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		GLuint vao;
		GLuint vbo;
		GLuint ebo;

		glDisable(GL_CULL_FACE);
		
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glDeleteBuffers(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void Renderer::configureDefaultShader() {
		glUseProgram(m_shaderManager.getDefaultShader());
	}
}