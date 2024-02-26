#include "../common.h"

#include "Renderer.h"
#include "Pathtracer.h"
#include "embree.h"

#include "GameObjectManager.h"
#include "Bootstrap.h"
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
	{
		static bool initialized = false;
		if (initialized) {
			Logger::log("Renderer already initialized, very bad!!", LOG_LEVEL_FATAL);
		}
		initialized = true;
		
		envmap_bg_id = m_textureManager.loadHdrTexture("001.hdr");
		envmap_refmap_id = m_textureManager.loadHdrTexture("001_dl_0.hdr");

		glActiveTexture(GL_TEXTURE0 + 5);
		glBindTexture(GL_TEXTURE_2D, envmap_bg_id);
		glActiveTexture(GL_TEXTURE0 + 6);
		glBindTexture(GL_TEXTURE_2D, envmap_refmap_id);


		///////////////////////////////////////////////////////////////////////////
		// Generate result texture
		///////////////////////////////////////////////////////////////////////////
		glGenTextures(1, &m_pathtracerTxtId);
		glActiveTexture(GL_TEXTURE0 + 7);
		glBindTexture(GL_TEXTURE_2D, m_pathtracerTxtId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		///////////////////////////////////////////////////////////////////////////
		// Initial path-tracer settings
		///////////////////////////////////////////////////////////////////////////
		PathTracer::settings.max_bounces = 8;
		PathTracer::settings.max_paths_per_pixel = 0; // 0 = Infinite
		PathTracer::settings.subsampling = 4;

		///////////////////////////////////////////////////////////////////////////
		// Set up light sources
		///////////////////////////////////////////////////////////////////////////
		PathTracer::point_light.intensity_multiplier = 2500.0f;
		PathTracer::point_light.color = vec3(1.f, 1.f, 1.f);
		PathTracer::point_light.position = vec3(10.0f, 25.0f, 20.0f);

		///////////////////////////////////////////////////////////////////////////
		// Load environment map
		///////////////////////////////////////////////////////////////////////////
		//todo, use texturemanager
		PathTracer::environment.map.load("../Shard/res/envmaps/001.hdr");
		PathTracer::environment.multiplier = 1.0f;

	}

	void Renderer::render() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, (int)(m_resolution.x), (int)(m_resolution.y));

		// i have no fucking idea what this does (???)
		// stolen from old codebase
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_projectionMatrix = glm::perspective(m_sceneManager.camera.fov, m_resolution.x / m_resolution.y, 1.f, 300.f);

		drawScene();



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

	void Renderer::drawScene() {
		static bool loadedObjs = false;
		if (!loadedObjs && m_usePathTracing) {
			PathTracer::reinitScene();

			// Add models to PathTracer scene

			auto& gobs = GameObjectManager::getInstance().getObjects();
			for (auto& gob : gobs)
			{
				PathTracer::addModel(gob->m_model.get(), gob->m_model->getModelMatrix());
			}
			PathTracer::buildBVH();
			PathTracer::restart();
			loadedObjs = true;
		}
	
		if (m_usePathTracing) {
			//assuming initializing and all objects have been added
			drawPathTracedScene();
		}
		else {
			drawBackground(); // <-- draw it last always
			drawModels();
		}
	}

	void Renderer::drawPathTracedScene(){

		{ ///////////////////////////////////////////////////////////////////////
			// If first frame, or window resized, or subsampling changes,
			// inform the PathTracer
			///////////////////////////////////////////////////////////////////////
			int w, h;
			glfwGetWindowSize(m_window, &w, &h);
			static int old_subsampling;
			if (m_windowWidth != w || m_windowHeight != h || old_subsampling != PathTracer::settings.subsampling)
			{
				PathTracer::resize(w, h);
				m_windowWidth = w;
				m_windowWidth = h;
				old_subsampling = PathTracer::settings.subsampling;
			}
		}

		///////////////////////////////////////////////////////////////////////////
		// Trace one path per pixel
		///////////////////////////////////////////////////////////////////////////
		auto& camera = SceneManager::getInstance().camera;
		mat4 viewMatrix = glm::lookAt(camera.pos, camera.pos + camera.front, camera.worldUp);
		mat4 projMatrix = glm::perspective(radians(45.0f),
			float(PathTracer::rendered_image.width)
			/ float(PathTracer::rendered_image.height),
			0.1f, 100.0f);
		PathTracer::tracePaths(viewMatrix, projMatrix);

		///////////////////////////////////////////////////////////////////////////
		// Copy pathtraced image to texture for display
		///////////////////////////////////////////////////////////////////////////
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_pathtracerTxtId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, PathTracer::rendered_image.width,
			PathTracer::rendered_image.height, 0, GL_RGB, GL_FLOAT, PathTracer::rendered_image.getPtr());

		///////////////////////////////////////////////////////////////////////////
		// Render a fullscreen quad, textured with our pathtraced image.
		///////////////////////////////////////////////////////////////////////////
		glViewport(0, 0, m_windowWidth, m_windowHeight);
		glClearColor(0.1f, 0.1f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		//SDL_GetWindowSize(g_window, &windowWidth, &windowHeight);
		glfwGetWindowSize(m_window, &m_windowWidth, &m_windowHeight);
		//glUseProgram(shaderProgram);
		//labhelper::drawFullScreenQuad();


		static auto& sm = ShaderManager::getInstance();
		auto bg_shader = sm.getShader("copyTexture");
		glUseProgram(bg_shader);
		
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
	void Renderer::drawModels() {
		// TODO: this is fucked, they should come from sceneManager
		// but because models are not transforms this is fucked
		// physicsbodies should not have transforms, they should
		// be based on the model
		// TLDR: this is fucked
		auto& gobs = GameObjectManager::getInstance().getObjects();

		///////////////////
		// POINT LIGHT
		///////////////////
		// Note: No model is being drawn to show
		// the light source in the world.

		glm::vec3 light_color{ 1.0f, 1.0f, 1.0f };
		glm::vec3 light_position{ 0.0f, 10.0f, 0.0f };
		float light_ambient_intensity{ 0.1f };
		float light_diffuse_intensity{ 0.8f };
		float light_specular_intensity{ 1.0f };

		float attenuation_constant{ 1.0f };
		float attenuation_linear{ 0.0f };
		float attenuation_quadratic{ 0.001f };

		const GLuint defShader = m_shaderManager.getDefaultShader();
		glUseProgram(defShader);

		glm::mat4 viewMatrix = m_sceneManager.getCameraViewMatrix();
		auto VP = m_projectionMatrix * viewMatrix;
		auto camera_pos = m_sceneManager.camera.pos;

		m_shaderManager.SetMat4x4(defShader, glm::inverse(viewMatrix) ,"viewInverse");
		m_shaderManager.SetVec3(defShader, camera_pos, "u_ViewPosition");

		for (std::shared_ptr<GameObject> gob : gobs) {
			if (!gob->m_model->m_hasDedicatedShader) [[likely]] {
				configureDefaultShader();
			}

			glm::mat4 modelMatrix = gob->m_model->getModelMatrix();
			auto mvp = VP * modelMatrix;

			if (!gob->m_model->m_hasDedicatedShader) [[likely]] {
				// Vertex shader uniforms
				m_shaderManager.SetMat4x4(defShader, mvp, "u_MVP");
				m_shaderManager.SetMat4x4(defShader, modelMatrix, "u_ModelMatrix");
				// Fragment shader uniforms
				m_shaderManager.SetVec3(defShader, light_color, "u_LightColor");
				m_shaderManager.SetVec3(defShader, light_position, "u_LightPosition");
				m_shaderManager.SetFloat1(defShader, light_ambient_intensity, "u_LightAmbientIntensity");
				m_shaderManager.SetFloat1(defShader, light_diffuse_intensity, "u_LightDiffuseIntensity");
				m_shaderManager.SetFloat1(defShader, light_specular_intensity, "u_LightDpecularIntensity");
				//m_shaderManager.SetFloat1(defShader, attenuation_constant, "u_AttenuationConstant");
				//m_shaderManager.SetFloat1(defShader, attenuation_linear, "u_AttenuationLinear");
				//m_shaderManager.SetFloat1(defShader, attenuation_quadratic, "u_AttenuationQuadratic");
			}

			gob->m_model->Draw();

			if (Bootstrap::getEnvironmentVariable("physics_debug") == "1") { // if debug
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
		glm::mat4 ma = glm::translate(glm::mat4(1.0f), toDraw->m_model->position());
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
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUseProgram(0);
	}

	void Renderer::configureDefaultShader() {
		glUseProgram(m_shaderManager.getDefaultShader());
	}
}