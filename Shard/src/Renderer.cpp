#include "../common.h"

#include "Renderer.h"
#include "Pathtracer.h"
#include "embree.h"

#include "GameObjectManager.h"
#include "Bootstrap.h"
#include "Logger.h"
#include "Model.h"

#include "gtx/projection.hpp"

#include <filesystem>

#include <stb_image.h>

namespace Shard {

	Renderer::Renderer() 
		: m_resolution({ 1280, 760 })
		, m_nearPlane(1.f)
		, m_farPlane(10000.f)
		, m_drawColliders(true)
		, m_fieldOfView(45.f)
		, m_gui(nullptr)
		, m_heightfield(nullptr)
		, m_pathtracerTxtId(SIZE_MAX)
		, m_renderObjects({})
	{ }

	void Renderer::initialize(SceneManager& sceneManager,
		TextureManager& texManager,
		ShaderManager& shaderManager,
		GUI* gui,
		GLFWwindow* window)
		
	{
		static bool initialized = false;
		if (initialized) {
			Logger::log("Renderer already initialized, very bad!!", LOG_LEVEL_FATAL);
		}
		initialized = true;

		m_sceneManager = &sceneManager;
		m_textureManager = &texManager;
		m_shaderManager = &shaderManager;
		m_gui = gui;
		m_fieldOfView = sceneManager.camera.fov;
		m_projectionMatrix = glm::perspective(m_fieldOfView, m_resolution.x / m_resolution.y, m_nearPlane, m_farPlane);
		m_window = window;
		
		m_heightfield = new HeightField(sceneManager);

		loadRequiredShaders();

		envmap_bg_id = m_textureManager->loadHdrTexture("001.hdr");
		envmap_refmap_id = m_textureManager->loadHdrTexture("001_dl_0.hdr");
		envmap_irrmap_id = m_textureManager->loadHdrTexture("001_irradiance.hdr");

		//TODO, fix material values, now we have ice mountains
		//m_heightfield.loadHeightField("L3123F.png", "L3123F_downscaled.jpg", "L3123F_shininess.png");
		const float size = 10000.f;
		const float tesselation = 1000.f;

		m_heightfield->generateMesh(tesselation, size, 982374);

		glEnable(GL_DEPTH_TEST); // z-buffering
		glEnable(GL_CULL_FACE); // backface culling
	}

	void Renderer::addRenderObject(std::shared_ptr<RenderableObject> robj) {
		m_renderObjects.push_back(robj); 
	}

	void Renderer::loadRequiredShaders() {
		bool allow_errors = false;

#ifdef _DEBUG
		allow_errors = true;
#endif

		for (const auto& shader : m_requiredShaders) {
			m_shaderManager->loadShader(shader, allow_errors);
		}

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
		PathTracer::settings.max_bounces = 2;
		PathTracer::settings.max_paths_per_pixel = 0; // 0 = Infinite
		PathTracer::settings.subsampling = 4;

		///////////////////////////////////////////////////////////////////////////
		// Set up light sources
		///////////////////////////////////////////////////////////////////////////
		PathTracer::point_light.intensity_multiplier = 100.0f;
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
		/////////////////////////////////////////////////////////////
		// Reset viewport
		/////////////////////////////////////////////////////////////
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glViewport(0, 0, (int)(m_resolution.x), (int)(m_resolution.y));
		//glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/////////////////////////////////////////////////////////////
		// Render main pass
		/////////////////////////////////////////////////////////////

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, m_resolution.x, m_resolution.y);
		glClearColor(0.2f, 0.2f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawBackground();
		drawScene();

		for (const auto obj : m_renderObjects)
			obj->render();

		//drawGauges();

		/////////////////////////////////////////////////////////////
		// BORING STUFF!! TRUE!
		/////////////////////////////////////////////////////////////
		// Do this last, idk why
		m_gui->draw();

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
		
		glm::mat4 viewMatrix = m_sceneManager->getCameraViewMatrix();
		glm::mat4 mvpMatrix = m_projectionMatrix * viewMatrix;
		sm.SetMat4x4(shader, mvpMatrix, "u_MVP");

		glDepthFunc(GL_LEQUAL); // depth test passes when values are leq depth buffer's content
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_tex_id);
		cubemap_model->Draw();
		glDepthFunc(GL_LESS); // Set back to default
		glUseProgram(0);
	}

	void Renderer::drawBackground() {
		static auto& sm = ShaderManager::getInstance();
		auto bg_shader = sm.getShader("background");
		glUseProgram(bg_shader);

		glActiveTexture(GL_TEXTURE10); // DO NOT CHANGE TO 5 !!!!!! VERY BAD!!!
		glBindTexture(GL_TEXTURE_2D, envmap_bg_id);

		static float environment_multiplier = 1.0f;
		glm::mat4 viewMatrix = m_sceneManager->getCameraViewMatrix();
		auto VP = m_projectionMatrix * viewMatrix;
		auto camera_pos = m_sceneManager->camera.pos;

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
			m_heightfield->submitTriangles(m_sceneManager->getCameraViewMatrix(), m_projectionMatrix, envmap_bg_id, envmap_irrmap_id, envmap_refmap_id);
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
		mat4 projMatrix = glm::perspective(45.0f,
			float(PathTracer::rendered_image.width)
			/ float(PathTracer::rendered_image.height),
			0.1f, 100.0f);
		PathTracer::tracePaths(viewMatrix, projMatrix);

		///////////////////////////////////////////////////////////////////////////
		// Copy pathtraced image to texture for display
		///////////////////////////////////////////////////////////////////////////
		glActiveTexture(GL_TEXTURE7);
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
		auto& gobs = GameObjectManager::getInstance().getObjects();
		auto& sun = SceneManager::getInstance().sun;

		const auto shader = m_shaderManager->getDefaultShader();
		glUseProgram(shader);

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, envmap_bg_id);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, envmap_irrmap_id);
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, envmap_refmap_id);
		glActiveTexture(0);


		auto viewMatrix = m_sceneManager->getCameraViewMatrix();
		auto pv = m_projectionMatrix * viewMatrix;
		auto camera_pos = m_sceneManager->camera.pos;
		auto& sm = m_shaderManager;

		glm::vec4 viewSpaceLightPosition = viewMatrix * glm::vec4(m_sceneManager->sun.light_position, 1.0f);
		sm->SetVec3(shader, viewSpaceLightPosition, "viewSpaceLightPosition");
		sm->SetMat4x4(shader, glm::inverse(viewMatrix), "viewInverse");

		for (std::shared_ptr<GameObject> gob : gobs) {

			glm::mat4 modelMatrix = gob->m_model->getModelMatrix();
			auto mvp = pv * modelMatrix;


			if (!gob->m_model->m_hasDedicatedShader) [[likely]] {
				glm::mat4 normalMat = glm::inverse(glm::transpose(viewMatrix * modelMatrix));
				sm->SetMat4x4(shader, normalMat, "normalMatrix");
				sm->SetMat4x4(shader, viewMatrix * modelMatrix, "modelViewMatrix");
				sm->SetMat4x4(shader, mvp, "modelViewProjMatrix");
				sm->SetFloat1(shader, 1.f, "environment_multiplier");
				sm->SetFloat1(shader, 1000.f, "point_light_intensity_multiplier");
				
			}

			gob->m_model->Draw();
			if (Bootstrap::getEnvironmentVariable("physics_debug") == "1" || gob->m_drawCollider) { // if debug
				m_drawCollider(gob);
				glUseProgram(shader);
			}
		}

		glUseProgram(0);
	}

	void Renderer::m_drawCollider(std::shared_ptr<GameObject> toDraw) {

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
		glm::mat4 viewMatrix = m_sceneManager->getCameraViewMatrix();
		glm::mat4 mvpMatrix = m_projectionMatrix * viewMatrix;// *ma;

		auto shader = m_shaderManager->getShader("collider");
		glUseProgram(shader);

		m_shaderManager->SetVec3(shader, toDraw->m_body->m_debugColor, "colorIn");
		m_shaderManager->SetMat4x4(shader, mvpMatrix, "u_MVP");


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

	// smol cute cozy function :^)
	void Renderer::configureDefaultShader() {
		glUseProgram(m_shaderManager->getDefaultShader());
	}
}