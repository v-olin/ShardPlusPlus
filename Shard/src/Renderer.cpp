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
		, m_projectionMatrix(glm::perspective(m_fieldOfView, m_resolution.x / m_resolution.y, 1.f, 100.f))
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

			if (m_drawColliders) { // if debug
				drawCollider(gob);
			}
		}
	}

	void Renderer::drawCollider(std::shared_ptr<GameObject> toDraw) {
		return;
		// this is a problem for another day, very bad!!
		// auto transform = toDraw->body_->trans;
		// glm::vec3 position = transform->position();
		// glm::vec3 size = transform->size();
	}

	void Renderer::configureDefaultShader() {
		glUseProgram(m_shaderManager.getDefaultShader());
	}
}