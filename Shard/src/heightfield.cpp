
#include "heightfield.h"
#include <glm.hpp>
#include <gtx/transform.hpp>
//#include <labhelper.h>

#include <iostream>
#include <stdint.h>
#include <vector>
#include <stb_image.h>
#include <ranges>

#include "Logger.h"
#include "ShaderManager.h"
#include "GameObjectManager.h"

#include "noise.h"

using namespace glm;
using std::string;

namespace Shard {

	House::House(glm::vec3 pos, float size, int seed, float octaves, float mult) : GameObject()
		, m_position(pos)
		, m_vao(0)
		, m_size(size)
		, m_seed(seed)
		, m_octaves(octaves)
		, m_mult(mult)
	{
	}
	void House::initialize() {
		auto modelSize = m_model->size();


		float hx = modelSize.x / 2;
		float hz = modelSize.z / 2;


		float x = m_position.x, z = m_position.z;

		std::vector<float> cornerHeights{
			Noise::perlin(x - hx, z - hz, m_size, m_seed, m_octaves) * m_mult,
			Noise::perlin(x + hx, z - hz, m_size, m_seed, m_octaves) * m_mult,
			Noise::perlin(x - hx, z + hz, m_size, m_seed, m_octaves) * m_mult,
			Noise::perlin(x + hx, z + hz, m_size, m_seed, m_octaves) * m_mult
		};

		auto min = *std::ranges::min_element(cornerHeights);
		m_position.y = min;

	// translate matrix
		m_model->scale(glm::vec3(10.0f));
		m_model->translate(m_position);
	
		setPhysicsEnabled();
		m_body->m_mass = 10000000000.f;
		m_body->m_maxForce = glm::vec3{ 0.f };
		m_body->m_angularDrag = glm::vec3{ 10000.f };
		m_body->m_maxTorque = glm::vec3{0};
		m_body->m_drag = 100000.f;
		m_body->m_stopOnCollision = true;
		m_body->m_reflectOnCollision = true;
		m_body->m_impartForce = true;
		m_body->m_isKinematic = false;
		m_body->m_passThrough = false;
		m_body->m_usesGravity = false;
		m_body->m_clickable = false;
		m_drawCollider = true;
	
		m_body->m_bodyModel = m_model;
		m_body->setBoxCollider();
	
		GameObject::addTag("House");
	}
	
	/*
	void House::createGeometry() {
		const float hx = m_size.x / 2;
		const float hz = m_size.z / 2;
		const glm::vec3 pos = m_position;

		glm::vec3 min{ pos.x - hx, pos.y, pos.z - hz };
		glm::vec3 max{ pos.x + hx, pos.y + m_size.y, pos.z + hz };

		const std::vector<glm::vec3> vertices{
			{min.x,	min.y, max.z},	// v0
			{min.x,	min.y, min.z},	// v1
			{max.x,	min.y, min.z},	// v2
			{max.x,	min.y, max.z},	// v3
			{min.x,	max.y, max.z},	// v4
			{min.x,	max.y, min.z},	// v5
			{max.x,	max.y, min.z},	// v6
			{max.x,	max.y, max.z}	// v7
		};

		// whoever wrote these indices, git blame!!!
		const std::vector<int> indices{
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

		GLuint vbo, ibo;

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER,
			vertices.size() * sizeof(glm::vec3),
			vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			indices.size() * sizeof(int),
			indices.data(), GL_STATIC_DRAW);
	}

	void House::render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
		glDisable(GL_CULL_FACE);
		auto sm = ShaderManager::getInstance();
		auto shader = sm.getShader("simple");
		glUseProgram(shader);

		sm.SetMat4x4(shader, projectionMatrix * viewMatrix * glm::mat4(1.f), "modelViewProjectionMatrix");
		sm.SetVec3(shader, glm::vec3{ 0.2f, 0.3f, 0.8f }, "material_color"); // blue :^)

		glBindVertexArray(m_vao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glEnable(GL_CULL_FACE);
		glUseProgram(0);
	}
	*/

	HeightField::HeightField(SceneManager& scene_manager)//TODO, take path to all files used, maybe
		: m_meshResolution(0)
		, m_vao(UINT32_MAX)
		, m_positionBuffer(UINT32_MAX)
		, m_uvBuffer(UINT32_MAX)
		, m_indexBuffer(UINT32_MAX)
		, m_numIndices(0)
		, m_texid_hf(UINT32_MAX)
		, m_texid_diffuse(UINT32_MAX)
		, m_diffuseTexturePath("")
		, m_scene_manager(scene_manager)
		, m_customBaseColor(UINT32_MAX)
		, m_customRoughness(UINT32_MAX)
		, m_customNormals(UINT32_MAX)
		, m_texid_shininess(UINT32_MAX)
		, m_map_type(-1)
		, m_octaves(9)
	{
		/*
		loadHeightField(&m_texid_hf_goth, "../scenes/gothenburg/gothenburg_resized.jpg");
		loadHeightField(&m_texid_shininess_goth, "../scenes/gothenburg/gothenburg_shininess.png");
		loadDiffuseTexture(&m_texid_diffuse_goth, "../scenes/gothenburg/gothenburg_diffuse.png");
		*/

		loadCustomTextures();
	}
	
	void HeightField::loadHeightField(std::string heightFieldName, std::string diffuseName, std::string shinyName) {

		auto base_dir = "../Shard/res/heightfields/";
		loadHeightFieldTexture(&m_texid_hf, base_dir + heightFieldName);
		loadDiffuseTexture(&m_texid_shininess, base_dir + shinyName);
		loadDiffuseTexture(&m_texid_diffuse, base_dir + diffuseName);
		m_map_type = LOADED;
	}

	void HeightField::loadHeightFieldTexture(GLuint* target, const std::string& heigtFieldPath)
	{
		int width, height, components;
		stbi_set_flip_vertically_on_load(true);
		float* data = stbi_loadf(heigtFieldPath.c_str(), &width, &height, &components, 1);
		if(data == nullptr)
		{
			Logger::log("Failed to load heightFieldTextuire: " + heigtFieldPath, LOG_LEVEL_FATAL);
		}

		if(*target == UINT32_MAX)
		{
			glGenTextures(1, target);
		}
		glBindTexture(GL_TEXTURE_2D, *target);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// just one component (float)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, data); 

		stbi_image_free(data);

		std::cout << "Successfully loaded heigh field texture: " << heigtFieldPath << ".\n";
	}

		// inherited from GameObject
	void House::checkDestroyMe() {}
	void House::physicsUpdate() {}
	void House::prePhysicsUpdate() {}
	void House::update() {}
	void House::killMe() {}
	//Inherit from colldsionhandler
	void House::onCollisionEnter(std::shared_ptr<Shard::PhysicsBody> body) {}
	void House::onCollisionExit(std::shared_ptr<Shard::PhysicsBody> body) {}
	void House::onCollisionStay(std::shared_ptr<Shard::PhysicsBody> body) {}


	void HeightField::loadDiffuseTexture(GLuint* target, const std::string& diffusePath)
	{
		int width, height, components;
		stbi_set_flip_vertically_on_load(true);
		uint8_t* data = stbi_load(diffusePath.c_str(), &width, &height, &components, 3);
		if(data == nullptr)
		{
			std::cout << "Failed to load image: " << diffusePath << ".\n";
			return;
		}

		if(*target == UINT32_MAX)
		{
			glGenTextures(1, target);
		}

		glBindTexture(GL_TEXTURE_2D, *target);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // plain RGB
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);

		std::cout << "Successfully loaded diffuse texture: " << diffusePath << ".\n";
	}

	void HeightField::loadTexture(GLuint* target, const std::string& path)
	{
		int w, h, c;
		stbi_set_flip_vertically_on_load(true);
		uint8_t* data = stbi_load(path.c_str(), &w, &h, &c, 3);

		if (data == nullptr)
		{
			Logger::log("Could not load texture: " + path + "Heightfield", LOG_LEVEL_FATAL);
		}

		if (*target == UINT32_MAX)
			glGenTextures(1, target);

		glBindTexture(GL_TEXTURE_2D, *target);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // plain RGB
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);

	}

	void HeightField::loadCustomTextures()
	{
		loadTexture(&m_customBaseColor, "../Shard/res/textures/grass_basecolor.jpg");
		loadTexture(&m_customRoughness, "../Shard/res/textures/grass_rough.jpg");
		loadTexture(&m_customNormals, "../Shard/res/textures/grass_normal.jpg");
	}

	void HeightField::generateMesh(int tesselation, float size, int seed)
	{
		Logger::log("Generating mesh for heightfield");
		if(m_map_type == -1)
			m_map_type = GENERATED;

		std::vector<vec3> vertices{};
		std::vector<int> indices{};
		std::vector<vec3> normals{};
		std::vector<vec2> texcoords{};

		m_mapSize = size;
		float sideLen = size / (tesselation - 1);

		float totalX = 0.0f;
		float totalZ = 0.0f;
	
		for (int i = 0; i < tesselation; i++)
		{
			float x = -size / 2 + i * sideLen;

			for (int j = 0; j < tesselation; j++)
			{
				float z = -size / 2 + j * sideLen;
				float y = Noise::perlin(x, z, size, seed, m_octaves) * 70.f;
				
				vertices.push_back({ x, y, z });

				texcoords.push_back({ totalX / size, totalZ / size });

				if (i != tesselation - 1 && j != tesselation - 1)
				{
					indices.push_back(i * tesselation + j);
					indices.push_back(i * tesselation + j+1);
					indices.push_back((i + 1) * tesselation + j + 1);

					indices.push_back(i * tesselation + j);
					indices.push_back((i + 1) * tesselation + j + 1);
					indices.push_back((i + 1) * tesselation + j);
				}
				totalZ += sideLen;
			}
			totalZ = 0;
			totalX += sideLen;
		}

		for (int i = 0; i < tesselation; i++)
		{
			for (int j = 0; j < tesselation; j++)
			{
				//  	A	
				//  B	x	C
				//  	D
				vec3 x, a, b, c, d, n;
				x = vertices.at(i * tesselation + j);
			
				if (i == 0 && j == 0)
				{
					a = vertices.at((i + 1) * tesselation + j);
					c = vertices.at(i * tesselation + j + 1);
	
					n = cross(c - x, a - x);
				}
				else if (i == tesselation - 1 && j == tesselation - 1)
				{
					b = vertices.at(i * tesselation + j - 1);
					d = vertices.at((i - 1) * tesselation + j);
	
					n = cross(b - x, d - x);
				}
				else if (i == 0)
				{
					b = vertices.at(i * tesselation + j - 1);
					c = vertices.at(i * tesselation + j + 1);
					d = vertices.at((i + 1) * tesselation + j);
	
					n = cross(b - x, d - x) + cross(d - x, c - x);
				}
				else if (i == tesselation - 1)
				{
					a = vertices.at((i - 1) * tesselation + j);
					b = vertices.at(i * tesselation + j - 1);
					c = vertices.at(i * tesselation + j + 1);
	
					n = cross(a - x, b - x) + cross(c - x, a - x);
				}
				else if (j == 0)
				{
					a = vertices.at((i - 1) * tesselation + j);
					c = vertices.at(i * tesselation + j + 1);
					d = vertices.at((i + 1) * tesselation + j);
	
					n = cross(c - x, a - x) + cross(d - x, c - x);
				}
				else if (j == tesselation - 1)
				{
					a = vertices.at((i - 1) * tesselation + j);
					b = vertices.at(i * tesselation + j - 1);
					d = vertices.at((i + 1) * tesselation + j);
	
					n = cross(a - x, b - x) + cross(b - x, d - x);
				}
				else
				{
					a = vertices.at((i - 1) * tesselation + j);
					b = vertices.at(i * tesselation + j - 1);
					c = vertices.at(i * tesselation + j + 1);
					d = vertices.at((i + 1) * tesselation + j);
	
					n = cross(a - x, b - x)
						+ cross(b - x, d - x)
						+ cross(d - x, c - x)
						+ cross(c - x, a - x);
				}
				normals.push_back(-normalize(n));
				//normals.push_back(vec3(0.0, 1.0, 0.0));
			}
		}

		m_numIndices = indices.size();

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glGenBuffers(1, &m_positionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_positionBuffer);
		glBufferData(GL_ARRAY_BUFFER,
			vertices.size() * sizeof(vec3),
			vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(0);

		// normals
		glGenBuffers(1, &m_normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
		glBufferData(GL_ARRAY_BUFFER,
			normals.size() * sizeof(vec3),
			normals.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(1);
	
		glGenBuffers(1, &m_uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(vec2),
			texcoords.data(), GL_STATIC_DRAW);
	
		glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(2);
	

		glGenBuffers(1, &m_indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			indices.size() * sizeof(int),
			indices.data(), GL_STATIC_DRAW);


		m_shaderProgram = ShaderManager::getInstance().getShader("heightfield");

		m_houseModels =  {
			std::make_shared<Model>("models/city/ResidentialBuildings001.obj"),
			std::make_shared<Model>("models/city/ResidentialBuildings002.obj"),
			std::make_shared<Model>("models/city/ResidentialBuildings003.obj"),
			std::make_shared<Model>("models/city/ResidentialBuildings004.obj"),
			std::make_shared<Model>("models/city/ResidentialBuildings005.obj"),
			std::make_shared<Model>("models/city/ResidentialBuildings006.obj"),
			std::make_shared<Model>("models/city/ResidentialBuildings007.obj"),
			std::make_shared<Model>("models/city/ResidentialBuildings008.obj"),
			std::make_shared<Model>("models/city/ResidentialBuildings009.obj"),
			std::make_shared<Model>("models/city/ResidentialBuildings010.obj")
		};

		if (m_map_type == GENERATED) {
			// I BUILD THE HOUSES :DDDD
			for (int i = 0; i < 100; i++) {
				auto num = rand() % 10;
				//num = 1;
				float LO = -size / 2 + size * 0.01f;
				float HI = size / 2 - size * 0.01f;
				float x = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
				float z = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));

				// glm::vec3 pos, float size, int seed, float octaves, float mult
				auto house = std::make_shared<House>(
					glm::vec3{ x, 0.f, z },
					size, seed, m_octaves, 70.f
				);
				house->m_model = std::make_shared<Model>(m_houseModels[num]);
				house->initialize();
				GameObjectManager::getInstance().addGameObject(house);
			}
		}
	}

	void HeightField::submitTriangles(const glm::mat4& viewMatrix, 
		const glm::mat4& projectionMatrix,
		GLuint envMap, GLuint irradMap, GLuint refMap)
	{
		if(m_vao == UINT32_MAX) {
			Logger::log("No vertex array is generated, cannot draw anything.", LOG_LEVEL_FATAL);
			return;
		}

		glUseProgram(m_shaderProgram);

		if (m_map_type == -1) {
			Logger::log("No heighmap loaded or generated", LOG_LEVEL_FATAL);
		}

		if (m_map_type == LOADED) {
			glActiveTexture(GL_TEXTURE0); // <--- verify which unit to bind to
			glBindTexture(GL_TEXTURE_2D, m_texid_hf);
			glActiveTexture(GL_TEXTURE1); // <--- verify which unit to bind to
			glBindTexture(GL_TEXTURE_2D, m_texid_diffuse);
			glActiveTexture(GL_TEXTURE2); // <--- verify which unit to bind to
			glBindTexture(GL_TEXTURE_2D, m_texid_shininess);
		}
		else {
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, m_customBaseColor);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, m_customNormals);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, m_customRoughness);
		}

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, envMap);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, irradMap);
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, refMap);

		auto& sm = ShaderManager::getInstance();

		glm::mat4 mvp = projectionMatrix * viewMatrix * glm::mat4(1.f);
		sm.SetMat4x4(m_shaderProgram, mvp,  "modelViewProjectionMatrix");

		//sm.SetFloat1(m_shaderProgram, 1.f, "heightScale");

		sm.SetInteger1(m_shaderProgram, m_map_type, "mapType");

		sm.SetFloat1(m_shaderProgram, m_mapSize, "heightMapSize");

		// uniform mat4 viewInverse;
		sm.SetMat4x4(m_shaderProgram, inverse(viewMatrix), "viewInverse");

		// uniform vec3 viewSpaceLightPosition;
		glm::vec4 viewSpaceLightPosition = viewMatrix * glm::vec4(m_scene_manager.sun.light_position, 1.0f);
		sm.SetVec3(m_shaderProgram, vec3(viewSpaceLightPosition), "viewSpaceLightPosition");
	
		//glm::mat4 lightMatrix = translate(glm::vec3(0.5f)) * scale(glm::vec3(0.5f)) * lightProjectionMatrix * lightViewMatrix * inverse(viewMatrix);
		//sm.SetMat4x4(m_shaderProgram, lightMatrix, "lightMatrix");

		//uniform mat4 normalMatrix;
		sm.SetMat4x4(m_shaderProgram, inverse(transpose(viewMatrix)), "normalMatrix");
	
		//uniform mat4 modelViewMatrix;
		sm.SetMat4x4(m_shaderProgram, viewMatrix * glm::mat4(1.0f), "modelViewMatrix");

		// uniform float material_metalness;
		sm.SetFloat1(m_shaderProgram, metalness, "material_metalness");
	
		// uniform float material_fresnel;
		sm.SetFloat1(m_shaderProgram, fresnel, "material_fresnel");
	
		// uniform float material_shininess;
		sm.SetFloat1(m_shaderProgram, shininess, "material_shininess");

		glBindVertexArray(m_vao);
		glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);

		glUseProgram(0);

	/*	for (House &house : m_houses) {
			house.render(viewMatrix, projectionMatrix);
		}*/
	}

} // end namespace