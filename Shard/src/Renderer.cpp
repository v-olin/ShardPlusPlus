#include "../common.h"

#include "Renderer.h"

#include "GameObjectManager.h"
#include "Logger.h"

#include <filesystem>

#include <stb_image.h>

namespace Shard {

	Renderer::Renderer(SceneManager& sceneManager,
		TextureManager& texManager,
		ShaderManager& shaderManager,
		GUI& gui,
		GLFWwindow* window)
		: m_sceneManager(sceneManager)
		, m_textureManager(texManager)
		, m_shaderManager(shaderManager)
		, m_gui(gui)
		, m_resolution({ 1280, 760 })
		, m_fieldOfView(sceneManager.camera.fov)
		, m_projectionMatrix(glm::perspective(sceneManager.camera.fov, m_resolution.x / m_resolution.y, 1.f, 300.f))
		, m_drawColliders(true)
		, m_window(window)
		, m_heightfield(sceneManager)
	{
		static bool initialized = false;
		if (initialized) {
			Logger::log("Renderer already initialized, very bad!!", LOG_LEVEL_FATAL);
		}
		initialized = true;

		loadRequiredShaders();
		
		envmap_bg_id = m_textureManager.loadHdrTexture("001.hdr");
		envmap_refmap_id = m_textureManager.loadHdrTexture("001_dl_0.hdr");
		envmap_irrmap_id = m_textureManager.loadHdrTexture("001_irradiance.hdr");

		glActiveTexture(GL_TEXTURE0 + 5);
		glBindTexture(GL_TEXTURE_2D, envmap_bg_id);
		glActiveTexture(GL_TEXTURE0 + 6);
		glBindTexture(GL_TEXTURE_2D, envmap_refmap_id);

		//TODO, fix material values, now we have ice mountains
		m_heightfield.loadHeightField("L3123F.png", "L3123F_downscaled.jpg", "L3123F_shininess.png");
		m_heightfield.generateMesh(500, 500, 0);

		m_shadowMapFB.resize(1024, 1024);
		glBindTexture(GL_TEXTURE_2D, m_shadowMapFB.depthBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

		glEnable(GL_DEPTH_TEST); // z-buffering
		glEnable(GL_CULL_FACE); // backface culling

	}

	void Renderer::loadRequiredShaders() {
		bool allow_errors = false;

#ifdef _DEBUG
		allow_errors = true;
#endif

		for (const auto& shader : m_requiredShaders) {
			m_shaderManager.loadShader(shader, allow_errors);
		}
	}

	void Renderer::render() {
		/////////////////////////////////////////////////////////////
		// Reset viewport
		/////////////////////////////////////////////////////////////
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, (int)(m_resolution.x), (int)(m_resolution.y));
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// TODO: this is very weird!! need to recompute it every frame anyway!!
		m_projectionMatrix = glm::perspective(m_sceneManager.camera.fov, m_resolution.x / m_resolution.y, m_nearPlane, m_farPlane);

		glm::mat4 camViewMat = glm::lookAt(
			m_sceneManager.camera.pos,
			m_sceneManager.camera.pos + m_sceneManager.camera.front,
			glm::vec3(0.f, 1.f, 0.f)
		);

		glm::mat4 camProjMat = glm::perspective(
			glm::radians(45.f),
			float(m_resolution.x) / float(m_resolution.y),
			m_nearPlane, m_farPlane
		);

		glm::mat4 sunViewMat = glm::lookAt(
			m_sceneManager.sun.light_position,
			glm::vec3(0.f),
			glm::vec3(0.f, 1.f, 0.f) // this may be fucked!!
		);

		glm::mat4 sunProjMat = glm::perspective(
			glm::radians(45.f),
			1.f, m_nearPlane, m_farPlane
		);

		/////////////////////////////////////////////////////////////
		// Draw shadow map
		/////////////////////////////////////////////////////////////
		
		glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFB.frameBufferId);
		glViewport(0, 0, m_shadowMapFB.width, m_shadowMapFB.height);
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		drawScene(sunViewMat, sunProjMat, true);
		
		
		/////////////////////////////////////////////////////////////
		// Render main pass
		/////////////////////////////////////////////////////////////

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, m_resolution.x, m_resolution.y);
		glClearColor(0.2f, 0.2f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawBackground();
		drawScene(camViewMat, camProjMat);

		/////////////////////////////////////////////////////////////
		// BORING STUFF!! TRUE!
		/////////////////////////////////////////////////////////////
		// Do this last, idk why
		m_gui.draw();

		// should check for errors here
		// surely there are no errors
		// very bad!!
		glfwSwapBuffers(m_window);
	}

	GLuint Renderer::LoadCubeMap(std::string cubemap_name)
	{

		cubemap_model = new CubeMap();

		auto base_dir = "../Shard/res/cubemaps/" + cubemap_name + "/";
		static std::vector<std::string> faces
		{
			base_dir + "right.jpg",
			base_dir + "left.jpg",
			base_dir + "top.jpg",
			base_dir + "bottom.jpg",
			base_dir + "front.jpg",
			base_dir + "back.jpg"
		};

		glGenTextures(1, &cubemap_tex_id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_tex_id);

		int width, height, nof_channels;
		unsigned char* data;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			data = stbi_load(faces[i].c_str(), &width, &height, &nof_channels, 0);
			if (data) {
				glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
			}
			else {
				Logger::log("[LOG][Renderer::LoadCubeMap] stbi_load failed");
				Logger::log((faces[i] + ", " + std::to_string(width) + ", " + std::to_string(height) + ", " + std::to_string(nof_channels)).c_str());
				Logger::log("-------------------------------");
				stbi_image_free(data);
				std::exit(1);
			}
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		Logger::log(("Loaded cubemap: " + cubemap_name).c_str());

	}

	void Renderer::drawCubeMap()
	{
		static auto& sm = ShaderManager::getInstance();
		auto shader = sm.getShader("cubemap");
		
		glUseProgram(shader);
		
		glm::mat4 viewMatrix = m_sceneManager.getCameraViewMatrix();
		glm::mat4 mvpMatrix = m_projectionMatrix * glm::mat4(glm::mat3(viewMatrix));
		sm.SetMat4x4(shader, mvpMatrix, "u_MVP");

		glDepthFunc(GL_LEQUAL); // depth test passes when values are leq depth buffer's content
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_tex_id);
		cubemap_model->Draw();
		glDepthFunc(GL_LESS); // Set back to default
		glUseProgram(0);
	}

	void Renderer::drawBackground() {
		static auto& sm = ShaderManager::getInstance();
		auto bg_shader = sm.getShader("background");
		glUseProgram(bg_shader);

		static float environment_multiplier = 1.0f;
		glm::mat4 viewMatrix = m_sceneManager.getCameraViewMatrix();
		auto VP = m_projectionMatrix * viewMatrix;
		auto camera_pos = m_sceneManager.camera.pos;

		sm.SetFloat1(bg_shader, environment_multiplier, "environment_multiplier");
		sm.SetMat4x4(bg_shader, inverse(VP), "inv_PV");
		sm.SetVec3(bg_shader, camera_pos,  "camera_pos");
		
		GLboolean previous_depth_state;
		glGetBooleanv(GL_DEPTH_TEST, &previous_depth_state);
		glDisable(GL_DEPTH_TEST);

		static GLuint VAO = 0;
		static GLuint VBO = 0;
		static int nofVertices = 6;
		
		if (VAO == 0)
		{ // do this initialization first time the function is called.
			static const float positions[] = { -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
												   -1.0f, -1.0f, 1.0f, 1.0f,  -1.0f, 1.0f };
			
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);
			
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), positions, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
			glEnableVertexAttribArray(0);
			
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

		}

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, nofVertices);

		if (previous_depth_state)
			glEnable(GL_DEPTH_TEST);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glUseProgram(0);
	}

	void Renderer::drawScene(glm::mat4 viewMatrix, glm::mat4 projMatrix, bool drawingShadowMap) {
		m_heightfield.submitTriangles(viewMatrix, projMatrix, envmap_bg_id, envmap_irrmap_id, envmap_refmap_id, m_shadowMapFB.frameBufferId, drawingShadowMap);
		drawModels(viewMatrix, projMatrix, drawingShadowMap);
	}

	void Renderer::drawModels(glm::mat4 viewMatrix, glm::mat4 projMatrix, bool drawingShadowMap) {
		auto& gobs = GameObjectManager::getInstance().getObjects();

		auto& sun = SceneManager::getInstance().sun;
		const GLuint shader = drawingShadowMap
									? m_shaderManager.getShader("shadowMap")
									: m_shaderManager.getDefaultShader();

		glUseProgram(shader);

		auto pv = projMatrix * viewMatrix;
		auto camera_pos = m_sceneManager.camera.pos;

		// copy paste, very bad!! ^v^ uwu
		glm::mat4 sunViewMat = glm::lookAt(
			m_sceneManager.sun.light_position,
			glm::vec3(0.f),
			glm::vec3(0.f, 1.f, 0.f) // this may be fucked!!
		);

		glm::mat4 sunProjMat = glm::perspective(
			glm::radians(45.f),
			1.f, m_nearPlane, m_farPlane
		);

		if (!drawingShadowMap) {
			m_shaderManager.SetMat4x4(shader, glm::inverse(viewMatrix) ,"viewInverse");
			glActiveTexture(GL_TEXTURE9);
			glBindTexture(GL_TEXTURE_2D, m_shadowMapFB.depthBuffer);

			glm::mat4 lightMatrix =
				glm::translate(glm::vec3(0.5f)) * glm::scale(glm::vec3(0.5f)) * sunProjMat * sunViewMat * glm::inverse(viewMatrix);
			glm::vec4 viewSpaceLightPosition = viewMatrix * glm::vec4(m_sceneManager.sun.light_position, 1.0f);

			m_shaderManager.SetVec3(shader, viewSpaceLightPosition, "viewSpaceLightPosition");
			m_shaderManager.SetMat4x4(shader, lightMatrix, "lightMatrix");
		}

		for (std::shared_ptr<GameObject> gob : gobs) {

			glm::mat4 modelMatrix = gob->m_model->getModelMatrix();
			auto mvp = pv * modelMatrix;

			m_shaderManager.SetMat4x4(shader, mvp, "modelViewProjMat");

			if (!drawingShadowMap && !gob->m_model->m_hasDedicatedShader) {
				m_shaderManager.SetMat4x4(shader, viewMatrix * modelMatrix, "modelViewMatrix");
				glm::mat4 normalMat = glm::inverse(glm::transpose(viewMatrix * modelMatrix));
				m_shaderManager.SetMat4x4(shader, normalMat, "normalMatrix");

				/* OLD UNIFORMS
				// Vertex shader uniforms
				m_shaderManager.SetMat4x4(shader, viewMatrix, "u_ViewMatrix");
				m_shaderManager.SetMat4x4(shader, mvp, "u_MVP");
				m_shaderManager.SetMat4x4(shader, modelMatrix, "u_ModelMatrix");
				// Fragment shader uniforms
				m_shaderManager.SetVec3(shader, sun.light_color, "u_LightColor");
				m_shaderManager.SetVec3(shader, sun.light_position, "u_LightPosition");
				m_shaderManager.SetFloat1(shader, sun.light_ambient_intensity, "u_LightAmbientIntensity");
				m_shaderManager.SetFloat1(shader, sun.light_diffuse_intensity, "u_LightDiffuseIntensity");
				m_shaderManager.SetFloat1(shader, sun.light_specular_intensity, "u_LightDpecularIntensity");
				m_shaderManager.SetFloat1(shader, sun.attenuation_constant, "u_AttenuationConstant");
				m_shaderManager.SetFloat1(shader, sun.attenuation_linear, "u_AttenuationLinear");
				m_shaderManager.SetFloat1(shader, sun.attenuation_quadratic, "u_AttenuationQuadratic");
				*/
			}

			gob->m_model->Draw();

			if (false) { // if debug
				drawCollider(gob);
			}
		}

		glUseProgram(0);
	}

	void Renderer::drawCollider(std::shared_ptr<GameObject> toDraw) {

		std::vector<glm::vec2> minMax = toDraw->m_body->m_collider->getTransformedMinMaxDims();

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
			min.x,	min.y, max.z,	// v0
			min.x,	min.y, min.z,	// v1
			max.x,	min.y, min.z,	// v2
			max.x,	min.y, max.z,	// v3
			min.x,	max.y, max.z,	// v4
			min.x,	max.y, min.z,	// v5
			max.x,	max.y, min.z,	// v6
			max.x,	max.y, max.z	// v7
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
		auto ma = glm::translate(glm::mat4(1.0f), toDraw->m_model->position());
		glm::mat4 viewMatrix = m_sceneManager.getCameraViewMatrix();
		glm::mat4 mvpMatrix = m_projectionMatrix * viewMatrix;// *ma;

		auto shader = m_shaderManager.getShader("collider");
		glUseProgram(shader);

		m_shaderManager.SetVec3(shader, toDraw->m_body->m_debugColor, "colorIn");
		m_shaderManager.SetMat4x4(shader, mvpMatrix, "u_MVP");

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		static GLuint vao = 0;
		static GLuint vbo = 0;
		static GLuint ebo = 0;

		glDisable(GL_CULL_FACE);
		
		if (vao == 0)
			glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		if (vbo == 0)
			glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(0);

		if (ebo == 0)
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