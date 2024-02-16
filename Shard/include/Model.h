// Model.h - Description
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <glm.hpp>

namespace Shard {

	struct Texture {
		bool valid = false;
		uint32_t gl_id = 0;
		uint32_t gl_id_internal = 0;
		std::string filename;
		std::string directory;
		int width, height;
		uint8_t* data;
		uint8_t n_components = 4;

		bool load(const std::string& directory, const std::string& filename, int nof_component);
		glm::vec4 sample(glm::vec2 uv) const;
		void free();

	};

	struct Material	{
		std::string m_name;
		glm::vec3 m_color;
		float m_shininess;
		float m_metalness;
		float m_fresnel;
		glm::vec3 m_emission;
		float m_transparency;
		float m_ior;
		Texture m_color_texture;
		Texture m_shininess_texture;
		Texture m_metalness_texture;
		Texture m_fresnel_texture;
		Texture m_emission_texture;
	};

	struct Mesh	{
		std::string m_name;
		uint32_t m_material_idx;
		// Where this Mesh's vertices start
		uint32_t m_start_index;
		uint32_t m_number_of_vertices;
	};

	class Model	{
	public:
		Model(std::string path);
		~Model();

		glm::vec3 position();
		glm::vec3 rotation();
		glm::vec3 size();
		glm::vec3 getLastDirection();
		glm::mat3 getRotationMatrix();

		void Draw();
		void translate(const glm::vec3& force);
		void rotate(const float angle_deg, const glm::vec3& axis);
		void scale(const glm::vec3& scale);
		glm::mat4 getModelMatrix();

		// The name of the whole model
		std::string m_name;
		// The filename of this model
		std::string m_filename;
		// The materials
		std::vector<Material> m_materials;
		// A model will contain one or more "Meshes"
		std::vector<Mesh> m_meshes;
		// Buffers on CPU
		std::vector<glm::vec3> m_positions;
		std::vector<glm::vec3> m_normals;
		std::vector<glm::vec2> m_texture_coordinates;
		// Buffers on GPU
		uint32_t m_positions_bo;
		uint32_t m_normals_bo;
		uint32_t m_texture_coordinates_bo;
		// Vertex Array Object
		uint32_t m_vaob;
		bool m_hasDedicatedShader;

		// merged from transform
		//glm::mat4 m_transformMatrix;
		//glm::mat4 m_lastTransformMatrix;

		glm::mat4 m_transMatrix;
		glm::mat4 m_rotMatrix;
		glm::mat4 m_lastTransMatrix;

		glm::vec3 m_forward;
		glm::vec3 m_up;
		glm::vec3 m_right;

		glm::vec3 max;
		glm::vec3 min;
	};

	void saveModelToOBJ(Model* model, std::string filename);
	void saveModelMaterialsToMTL(Model* model, std::string filename);

	// no implementation, very bad!!
	void freeModel(Model* model);
	void render(const Model* model, const bool submitMaterials = true);

}
