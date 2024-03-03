#include <string>
#include "../common.h"
#include "glm.hpp"
#include "SceneManager.h"

namespace Shard {
	enum MapType {
		LOADED,
		GENERATED
	};

	class House {
	public:
		House(glm::vec3 pos, float size, int seed, float octaves, float mult);

		void render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
	private:
		GLuint m_vao;
		glm::vec3 m_position;
		glm::vec3 m_size;

		void createGeometry();
	};
		
	class HeightField {
	public:
		int m_meshResolution; // triangles edges per quad side
		GLuint m_texid_hf;
		GLuint m_texid_diffuse;
		GLuint m_texid_shininess;
		GLuint m_vao;
		GLuint m_positionBuffer;
		GLuint m_uvBuffer;
		GLuint m_indexBuffer;
		GLuint m_numIndices;
		GLuint m_normalBuffer;
		std::string m_diffuseTexturePath;
		GLuint m_shaderProgram;

		float m_mapSize;
		float metalness{ 1.f };
		float fresnel{ 1.f };
		float shininess{ 1.f };

		//GLfloat water_metalness = 0.62f;
		//GLfloat water_fresnel = 0.68f;
		//GLfloat water_shininess = 5300.0f;
		float m_octaves;
		float m_height_scale;
		int m_map_type;

		GLuint m_customBaseColor, m_customRoughness, m_customNormals;

		HeightField(SceneManager& scenemanager);

		void loadHeightField(std::string heightFieldName, std::string diffuseName, std::string shinyName);

		/// Load height field
		void loadHeightFieldTexture(GLuint* target, const std::string& heigtFieldPath);

		/// Load diffuse map
		void loadDiffuseTexture(GLuint* target, const std::string& diffusePath);

		void loadCustomTextures(void);

		/// Generate mesh
		void generateMesh(int tesselation, float size, int seed);

		/// Render height map
		void submitTriangles(const glm::mat4& viewMatrix,
			const glm::mat4& projectionMatrix,
			GLuint envMap, GLuint irradMap, GLuint refMap);

	private:
		std::vector<House> m_houses{};
		
		//SceneContext& sceneContext;
		SceneManager& m_scene_manager;
		void loadTexture(GLuint* target, const std::string& path);
	};

}
