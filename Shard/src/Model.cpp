#include "Model.h"

#include "common.h"
#include "Logger.h"

#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#include "ShaderManager.h"

#include <gtx/transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include "gtc/matrix_transform.hpp"

// DON'T REMOVE DEFINES, AND DON'T DEFINE ANYWHERE ELSE!!!!!!!!!!!!!
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <filesystem>
#include <limits>


#define MAX std::numeric_limits<float>::max()
#define MIN std::numeric_limits<float>::min()
namespace Shard {

	namespace file_util {

		std::string normalise(const std::string& file_name);
		std::string parent_path(const std::string& file_name);
		std::string file_stem(const std::string& file_name);
		std::string file_extension(const std::string& file_name);
		std::string change_extension(const std::string& file_name, const std::string& ext);

		std::string normalise(const std::string& file_name)
		{
			std::string nname;
			nname.reserve(file_name.size());
			for (const char c : file_name)
			{
				if (c == '\\')
				{
					if (nname.back() != '/')
					{
						nname += '/';
					}
				}
				else
				{
					nname += c;
				}
			}

			return nname;
		}

		std::string file_stem(const std::string& file_name)
		{
			size_t slash = file_name.find_last_of("\\/");
			size_t dot = file_name.find_last_of(".");
			if (slash != std::string::npos)
			{
				return file_name.substr(slash + 1, dot - slash - 1);
			}
			else
			{
				return file_name.substr(0, dot);
			}
		}

		std::string file_extension(const std::string& file_name)
		{
			size_t separator = file_name.find_last_of(".");
			if (separator == std::string::npos)
			{
				return "";
			}
			else
			{
				return file_name.substr(separator);
			}
		}

		std::string change_extension(const std::string& file_name, const std::string& ext)
		{
			size_t separator = file_name.find_last_of(".");
			if (separator == std::string::npos)
			{
				return file_name + ext;
			}
			else
			{
				return file_name.substr(0, separator) + ext;
			}
		}

		std::string parent_path(const std::string& file_name)
		{
			size_t separator = file_name.find_last_of("\\/");
			if (separator != std::string::npos)
			{
				return file_name.substr(0, separator + 1);
			}
			else
			{
				return "./";
			}
		}

	}

	void Texture::free()
	{
		if (data)
		{
			stbi_image_free(data);
			data = nullptr;
		}
		if (gl_id_internal)
		{
			glDeleteTextures(1, &gl_id_internal);
			gl_id_internal = 0;
		}
	}

	bool Texture::load(const std::string& _directory, const std::string& _filename, int _components)
	{
		filename = file_util::normalise(_filename);
		directory = file_util::normalise(_directory);
		valid = true;
		int components;
		data = stbi_load((directory + filename).c_str(), &width, &height, &components, _components);
		if (data == nullptr)
		{
			Logger::log("ERROR: loadModelFromOBJ(): Failed to load texture: " + filename + " in " + directory, LOG_LEVEL_FATAL);
			//std::cout << "ERROR: loadModelFromOBJ(): Failed to load texture: " << filename << " in " << directory
			//	<< "\n";
			//exit(1);
		}
		glGenTextures(1, &gl_id_internal);
		gl_id = gl_id_internal;
		glBindTexture(GL_TEXTURE_2D, gl_id_internal);
		GLenum format, internal_format;
		n_components = _components;
		if (_components == 1)
		{
			format = GL_R;
			internal_format = GL_R8;
		}
		else if (_components == 3)
		{
			format = GL_RGB;
			internal_format = GL_RGB;
		}
		else if (_components == 4)
		{
			format = GL_RGBA;
			internal_format = GL_RGBA;
		}
		else
		{
			Logger::log("Texture loading not implemented for this number of compenents.\n", LOG_LEVEL_FATAL);
			//std::cout << "Texture loading not implemented for this number of compenents.\n";
			//exit(1);
		}
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);

		glBindTexture(GL_TEXTURE_2D, 0);
		return true;
	}

	glm::vec4 Texture::sample(glm::vec2 uv) const
	{
		int x = int(uv.x * width + 0.5) % width;
		int y = int(uv.y * height + 0.5) % height;
		if (n_components == 4)
		{
			return glm::vec4(data[(y * width + x) * 4 + 0], data[(y * width + x) * 4 + 1],
				data[(y * width + x) * 4 + 2], data[(y * width + x) * 4 + 3])
				/ 255.f;
		}
		else
		{
			// Just return one channel
			return glm::vec4(data[(y * width + x) * n_components + 0], data[(y * width + x) * n_components + 0],
				data[(y * width + x) * n_components + 0], data[(y * width + x) * n_components + 0])
				/ 255.f;
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Destructor
	///////////////////////////////////////////////////////////////////////////
	Model::~Model()
	{
		for (auto& material : m_materials)
		{
			if (material.m_color_texture.valid)
				material.m_color_texture.free();
			if (material.m_shininess_texture.valid)
				material.m_shininess_texture.free();
			if (material.m_metalness_texture.valid)
				material.m_metalness_texture.free();
			if (material.m_fresnel_texture.valid)
				material.m_fresnel_texture.free();
			if (material.m_emission_texture.valid)
				material.m_emission_texture.free();
		}
		glDeleteBuffers(1, &m_positions_bo);
		glDeleteBuffers(1, &m_normals_bo);
		glDeleteBuffers(1, &m_texture_coordinates_bo);
	}


	Model::Model(std::string path)
		: m_hasDedicatedShader(false)
		//, m_transformMatrix(glm::mat4(1.f))
		//, m_lastTransformMatrix(glm::mat4(1.f))
		, m_transMatrix(glm::mat4(1.f))
		, m_rotMatrix(glm::mat4(1.f))
		, m_lastTransMatrix(glm::mat4(1.f))
		, m_forward({ 1.f, 0, 0 })
		, m_up({ 0, 1.f, 0})
		, m_right({ 0, 0, 1.f})
		, max(MIN)
		, min(MAX)
	{
		std::string filename, extension, directory;

		filename = file_util::normalise(path);
		directory = file_util::parent_path(path);
		filename = file_util::file_stem(path);
		extension = file_util::file_extension(path);


		if (extension != ".obj")
		{
			std::cout << "Fatal: loadModelFromOBJ(): Expecting filename ending in '.obj'\n";
			exit(1);
		}

		///////////////////////////////////////////////////////////////////////
		// Parse the OBJ file using tinyobj
		///////////////////////////////////////////////////////////////////////
		std::cout << "Loading " << path << "..." << std::flush;
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn;
		std::string err;

		// Expect '.mtl' file in the same directory and triangulate meshes
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
			(directory + filename + extension).c_str(), directory.c_str(), true);
		
		// `err` may contain warning message.
		if (!err.empty())
			std::cerr << err << std::endl;

		if (!ret)
			exit(1);
		
		m_name = filename;
		m_filename = path;

		///////////////////////////////////////////////////////////////////////
		// Transform all materials into our datastructure
		///////////////////////////////////////////////////////////////////////
		for (const auto& m : materials)
		{
			Material material;
			material.m_name = m.name;
			material.m_color = glm::vec3(m.diffuse[0], m.diffuse[1], m.diffuse[2]);
			if (m.diffuse_texname != "")
			{
				material.m_color_texture.load(directory, m.diffuse_texname, 4);
			}
			material.m_metalness = m.metallic;
			if (m.metallic_texname != "")
			{
				material.m_metalness_texture.load(directory, m.metallic_texname, 1);
			}
			material.m_fresnel = m.specular[0];
			if (m.specular_texname != "")
			{
				material.m_fresnel_texture.load(directory, m.specular_texname, 1);
			}
			material.m_shininess = m.roughness;
			if (m.roughness_texname != "")
			{
				material.m_shininess_texture.load(directory, m.roughness_texname, 1);
			}
			material.m_emission = glm::vec3(m.emission[0], m.emission[1], m.emission[2]);
			if (m.emissive_texname != "")
			{
				material.m_emission_texture.load(directory, m.emissive_texname, 4);
			}
			material.m_transparency = m.transmittance[0];
			material.m_ior = m.ior;
			m_materials.push_back(material);
		}

		///////////////////////////////////////////////////////////////////////
		// A vertex in the OBJ file may have different indices for position,
		// normal and texture coordinate. We will not even attempt to use
		// indexed lookups, but will store a simple vertex stream per mesh.
		///////////////////////////////////////////////////////////////////////
		uint64_t number_of_vertices = 0;
		for (const auto& shape : shapes)
		{
			number_of_vertices += shape.mesh.indices.size();
		}
		m_positions.resize(number_of_vertices);
		m_normals.resize(number_of_vertices);
		m_texture_coordinates.resize(number_of_vertices);

		///////////////////////////////////////////////////////////////////////
		// For each vertex _position_ auto generate a normal that will be used
		// if no normal is supplied.
		///////////////////////////////////////////////////////////////////////
		std::vector<glm::vec4> auto_normals(attrib.vertices.size() / 3);
		for (const auto& shape : shapes)
		{
			for (int face = 0; face < int(shape.mesh.indices.size()) / 3; face++)
			{
				glm::vec3 v0 = glm::vec3(attrib.vertices[shape.mesh.indices[face * 3 + 0].vertex_index * 3 + 0],
										 attrib.vertices[shape.mesh.indices[face * 3 + 0].vertex_index * 3 + 1],
										 attrib.vertices[shape.mesh.indices[face * 3 + 0].vertex_index * 3 + 2]);
				glm::vec3 v1 = glm::vec3(attrib.vertices[shape.mesh.indices[face * 3 + 1].vertex_index * 3 + 0],
										 attrib.vertices[shape.mesh.indices[face * 3 + 1].vertex_index * 3 + 1],
										 attrib.vertices[shape.mesh.indices[face * 3 + 1].vertex_index * 3 + 2]);
				glm::vec3 v2 = glm::vec3(attrib.vertices[shape.mesh.indices[face * 3 + 2].vertex_index * 3 + 0],
										 attrib.vertices[shape.mesh.indices[face * 3 + 2].vertex_index * 3 + 1],
										 attrib.vertices[shape.mesh.indices[face * 3 + 2].vertex_index * 3 + 2]);

				glm::vec3 e0 = glm::normalize(v1 - v0);
				glm::vec3 e1 = glm::normalize(v2 - v0);
				glm::vec3 face_normal = cross(e0, e1);

				auto_normals[shape.mesh.indices[face * 3 + 0].vertex_index] += glm::vec4(face_normal, 1.0f);
				auto_normals[shape.mesh.indices[face * 3 + 1].vertex_index] += glm::vec4(face_normal, 1.0f);
				auto_normals[shape.mesh.indices[face * 3 + 2].vertex_index] += glm::vec4(face_normal, 1.0f);
			}
		}
		for (auto& normal : auto_normals)
		{
			normal = (1.0f / normal.w) * normal;
		}

		///////////////////////////////////////////////////////////////////////
		// Now we will turn all shapes into Meshes. A shape that has several
		// materials will be split into several meshes with unique names
		///////////////////////////////////////////////////////////////////////
		int vertices_so_far = 0;
		for (int s = 0; s < shapes.size(); ++s)
		{
			const auto& shape = shapes[s];
			///////////////////////////////////////////////////////////////////
			// The shapes in an OBJ file may several different materials.
			// If so, we will split the shape into one Mesh per Material
			///////////////////////////////////////////////////////////////////
			int next_material_index = shape.mesh.material_ids[0];
			int next_material_starting_face = 0;
			std::vector<bool> finished_materials(materials.size(), false);
			int number_of_materials_in_shape = 0;
			while (next_material_index != -1)
			{
				int current_material_index = next_material_index;
				int current_material_starting_face = next_material_starting_face;
				next_material_index = -1;
				next_material_starting_face = -1;
				// Process a new Mesh with a unique material
				Mesh mesh;
				mesh.m_name = shape.name + "_" + materials[current_material_index].name;
				mesh.m_material_idx = current_material_index;
				mesh.m_start_index = vertices_so_far;
				number_of_materials_in_shape += 1;

				uint64_t number_of_faces = shape.mesh.indices.size() / 3;
				for (int i = current_material_starting_face; i < number_of_faces; i++)
				{
					if (shape.mesh.material_ids[i] != current_material_index)
					{
						if (next_material_index >= 0)
							continue;
						else if (finished_materials[shape.mesh.material_ids[i]])
							continue;
						else
						{ // Found a new material that we have not processed.
							next_material_index = shape.mesh.material_ids[i];
							next_material_starting_face = i;
						}
					}
					else
					{
						///////////////////////////////////////////////////////
						// Now we generate the vertices
						///////////////////////////////////////////////////////
						for (int j = 0; j < 3; j++)
						{
							int v = shape.mesh.indices[i * 3 + j].vertex_index;
							m_positions[vertices_so_far + j] =
								glm::vec3(attrib.vertices[shape.mesh.indices[i * 3 + j].vertex_index * 3 + 0],
									attrib.vertices[shape.mesh.indices[i * 3 + j].vertex_index * 3 + 1],
									attrib.vertices[shape.mesh.indices[i * 3 + j].vertex_index * 3 + 2]);

							max = glm::max(m_positions[vertices_so_far + j], max);
							min = glm::min(m_positions[vertices_so_far + j], min);

							if (shape.mesh.indices[i * 3 + j].normal_index == -1)
							{
								// No normal, use the autogenerated
								m_normals[vertices_so_far + j] = glm::vec3(
									auto_normals[shape.mesh.indices[i * 3 + j].vertex_index]);
							}
							else
							{
								m_normals[vertices_so_far + j] =
									glm::vec3(attrib.normals[shape.mesh.indices[i * 3 + j].normal_index * 3 + 0],
										attrib.normals[shape.mesh.indices[i * 3 + j].normal_index * 3 + 1],
										attrib.normals[shape.mesh.indices[i * 3 + j].normal_index * 3 + 2]);
							}
							if (shape.mesh.indices[i * 3 + j].texcoord_index == -1)
							{
								// No UV coordinates. Use null.
								m_texture_coordinates[vertices_so_far + j] = glm::vec2(0.0f);
							}
							else
							{
								m_texture_coordinates[vertices_so_far + j] = glm::vec2(
									attrib.texcoords[shape.mesh.indices[i * 3 + j].texcoord_index * 2 + 0],
									attrib.texcoords[shape.mesh.indices[i * 3 + j].texcoord_index * 2 + 1]);
							}
						}
						vertices_so_far += 3;
					}
				}
				///////////////////////////////////////////////////////////////
				// Finalize and push this mesh to the list
				///////////////////////////////////////////////////////////////
				mesh.m_number_of_vertices = vertices_so_far - mesh.m_start_index;
				m_meshes.push_back(mesh);
				finished_materials[current_material_index] = true;
			}
			if (number_of_materials_in_shape == 1)
			{
				// If there's only one material, we don't need the material name in the mesh name
				m_meshes.back().m_name = shape.name;
			}
		}

		std::sort(m_meshes.begin(), m_meshes.end(),
			[](const Mesh& a, const Mesh& b) { return a.m_name < b.m_name; });

		///////////////////////////////////////////////////////////////////////
		// Upload to GPU
		///////////////////////////////////////////////////////////////////////
		glGenVertexArrays(1, &m_vaob);
		glBindVertexArray(m_vaob);
		glGenBuffers(1, &m_positions_bo);
		glBindBuffer(GL_ARRAY_BUFFER, m_positions_bo);
		glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(glm::vec3), &m_positions[0].x,
			GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(0);
		glGenBuffers(1, &m_normals_bo);
		glBindBuffer(GL_ARRAY_BUFFER, m_normals_bo);
		glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(glm::vec3), &m_normals[0].x,
			GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(1);
		glGenBuffers(1, &m_texture_coordinates_bo);
		glBindBuffer(GL_ARRAY_BUFFER, m_texture_coordinates_bo);
		glBufferData(GL_ARRAY_BUFFER, m_texture_coordinates.size() * sizeof(glm::vec2),
			&m_texture_coordinates[0].x, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		std::cout << "done.\n";
	}

	glm::vec3 Model::position() {
		return glm::vec3(m_transMatrix[3]);
	}

	glm::vec3 Model::rotation() {
		// get rotation matrix from model matrix:
		// https://math.stackexchange.com/questions/237369/given-this-transformation-matrix-how-do-i-decompose-it-into-translation-rotati

		// this should cut off 4th column and 4th row
		glm::mat3 rotMatrix = glm::mat3(getRotationMatrix());

		/*
		get euler angles from rotation matrix
		https://stackoverflow.com/questions/1996957/conversion-euler-to-matrix-and-matrix-to-euler

		rotMatrix = Ry(H)*Rx(P)*Rz(B)
			= | cos H*cos B+sin H*sin P*sin B  cos B*sin H*sin P-sin B*cos H  cos P*sin H |
			  |                   cos P*sin B                    cos B*cos P       -sin P |
			  | sin B*cos H*sin P-sin H*cos B  sin H*sin B+cos B*cos H*sin P  cos P*cos H |

		*/

		// x = [1], y = [2], z = [3]
		// x = P = arcsin(-rot[3][2])
		float x = asin(-rotMatrix[2].y);
		// y = H = arctan(rot[3][1]/rot[3][3])
		float y = atan2(rotMatrix[2].x, rotMatrix[2].z);
		// z = B = arctan(rot[1][2]/rot[2][2])
		float z = atan2(rotMatrix[0].y, rotMatrix[1].y);

		return { x, y, z };
	}

	glm::vec3 Model::size() {
		return glm::vec3{
			glm::length(glm::vec3(m_transMatrix[0])),
			glm::length(glm::vec3(m_transMatrix[1])),
			glm::length(glm::vec3(m_transMatrix[2]))
		};
	}

	glm::vec3 Model::getLastDirection() {
		glm::vec3 pos = glm::vec3(m_transMatrix[3]);
		glm::vec3 lastPos = glm::vec3(m_lastTransMatrix[3]);
		return lastPos - pos;
	}

	glm::mat3 Model::getRotationMatrix() {
		auto size = this->size();
		return glm::mat3(m_rotMatrix);
	}

	void Model::Draw() {
		glBindVertexArray(m_vaob);
		static ShaderManager& sm = ShaderManager::getInstance();
		for (auto& mesh : m_meshes) {
			const Material& mat = m_materials[mesh.m_material_idx];
			sm.SetVec3(sm.getDefaultShader(), mat.m_color, "u_ObjectColor");
			glDrawArrays(GL_TRIANGLES, mesh.m_start_index, (GLsizei)mesh.m_number_of_vertices);
		}
		glBindVertexArray(0);
	}

	void Model::translate(const glm::vec3& force) {
		m_lastTransMatrix = m_transMatrix;
		m_transMatrix = glm::translate(force) * m_transMatrix;
		//m_transMatrix = glm::translate(m_transMatrix, force);
	}

	// why is the angle in degrees and not radians? very bad!!
	void Model::rotate(const float angle_deg, const glm::vec3& axis) {
		m_rotMatrix = glm::rotate(glm::radians(angle_deg), axis) * m_rotMatrix;
		// m_transformMatrix = glm::rotate(m_transformMatrix, glm::radians(angle_deg), axis);
		glm::mat3 rot = getRotationMatrix();

		// we also need to rotate the axis vectors of the transform
		// TDIL this is fucked, unlucky
		m_forward = glm::normalize(rot * glm::vec3{ 1.f, 0, 0 });
		m_up = glm::normalize(rot * glm::vec3{ 0, 1.f, 0 });
		m_right = glm::normalize(rot * glm::vec3{ 0, 0, 1.f });
	}

	void Model::scale(const glm::vec3& scale) {
		m_transMatrix = glm::scale(m_transMatrix, scale);
	}

	glm::mat4 Model::getModelMatrix() {
		return m_transMatrix * m_rotMatrix;
	}

	void saveModelMaterialsToMTL(Model* model, std::string filename)
	{
		///////////////////////////////////////////////////////////////////////
		// Save Materials
		///////////////////////////////////////////////////////////////////////
		std::ofstream mat_file(filename);
		if (!mat_file.is_open())
		{
			std::cout << "Could not open file " << filename << " for writing.\n";
			return;
		}
		mat_file << "# Exported by Chalmers Graphics Group\n";
		for (const auto& mat : model->m_materials)
		{
			mat_file << "newmtl " << mat.m_name << "\n";
			mat_file << "Kd " << mat.m_color.x << " " << mat.m_color.y << " " << mat.m_color.z << "\n";
			mat_file << "Ks " << mat.m_fresnel << " " << mat.m_fresnel << " " << mat.m_fresnel << "\n";
			mat_file << "Pm " << mat.m_metalness << "\n";
			mat_file << "Pr " << mat.m_shininess << "\n";
			mat_file << "Ke " << mat.m_emission.x << " " << mat.m_emission.y << " " << mat.m_emission.z << "\n";
			mat_file << "Tf " << mat.m_transparency << " " << mat.m_transparency << " " << mat.m_transparency
				<< "\n";
			if (mat.m_ior != 1.f)
			{
				mat_file << "Ni " << mat.m_ior << "\n";
			}
			if (mat.m_color_texture.valid)
				mat_file << "map_Kd " << mat.m_color_texture.filename << "\n";
			if (mat.m_metalness_texture.valid)
				mat_file << "map_Pm " << mat.m_metalness_texture.filename << "\n";
			if (mat.m_fresnel_texture.valid)
				mat_file << "map_Ks " << mat.m_fresnel_texture.filename << "\n";
			if (mat.m_shininess_texture.valid)
				mat_file << "map_Pr " << mat.m_shininess_texture.filename << "\n";
			if (mat.m_emission_texture.valid)
				mat_file << "map_Ke " << mat.m_emission_texture.filename << "\n";
			mat_file << "\n";
		}
		mat_file.close();
	}

	void saveModelToOBJ(Model* model, std::string filename)
	{
		std::string directory;
		filename = file_util::normalise(filename);
		directory = file_util::parent_path(filename);
		filename = file_util::file_stem(filename);

		///////////////////////////////////////////////////////////////////////
		// Save Materials
		///////////////////////////////////////////////////////////////////////
		saveModelMaterialsToMTL(model, directory + filename + ".mtl");

		///////////////////////////////////////////////////////////////////////
		// Save geometry
		///////////////////////////////////////////////////////////////////////
		std::ofstream obj_file(directory + filename + ".obj");
		if (!obj_file.is_open())
		{
			std::cout << "Could not open file " << filename << " for writing.\n";
			return;
		}
		obj_file << "# Exported by Chalmers Graphics Group\n";
		obj_file << "mtllib " << filename << ".mtl\n";
		int vertex_counter = 1;
		for (auto mesh : model->m_meshes)
		{
			obj_file << "o " << mesh.m_name << "\n";
			obj_file << "g " << mesh.m_name << "\n";
			obj_file << "usemtl " << model->m_materials[mesh.m_material_idx].m_name << "\n";
			for (uint32_t i = mesh.m_start_index; i < mesh.m_start_index + mesh.m_number_of_vertices; i++)
			{
				obj_file << "v " << model->m_positions[i].x << " " << model->m_positions[i].y << " "
					<< model->m_positions[i].z << "\n";
			}
			for (uint32_t i = mesh.m_start_index; i < mesh.m_start_index + mesh.m_number_of_vertices; i++)
			{
				obj_file << "vn " << model->m_normals[i].x << " " << model->m_normals[i].y << " "
					<< model->m_normals[i].z << "\n";
			}
			for (uint32_t i = mesh.m_start_index; i < mesh.m_start_index + mesh.m_number_of_vertices; i++)
			{
				obj_file << "vt " << model->m_texture_coordinates[i].x << " " << model->m_texture_coordinates[i].y
					<< "\n";
			}
			int number_of_faces = mesh.m_number_of_vertices / 3;
			for (int i = 0; i < number_of_faces; i++)
			{
				obj_file << "f " << vertex_counter << "/" << vertex_counter << "/" << vertex_counter << " "
					<< vertex_counter + 1 << "/" << vertex_counter + 1 << "/" << vertex_counter + 1 << " "
					<< vertex_counter + 2 << "/" << vertex_counter + 2 << "/" << vertex_counter + 2 << "\n";
				vertex_counter += 3;
			}
		}
	}

	///////////////////////////////////////////////////////////////////////
	// Loop through all Meshes in the Model and render them
	///////////////////////////////////////////////////////////////////////
	//void render(const Model* model, const bool submitMaterials)
	//{
	//	GLint current_program = 0;
	//	glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
	//
	//	glBindVertexArray(model->m_vaob);
	//	for (auto& mesh : model->m_meshes)
	//	{
	//		if (submitMaterials)
	//		{
	//			const Material& material = model->m_materials[mesh.m_material_idx];
	//
	//			bool has_color_texture = material.m_color_texture.valid;
	//			bool has_metalness_texture = material.m_metalness_texture.valid;
	//			bool has_fresnel_texture = material.m_fresnel_texture.valid;
	//			bool has_shininess_texture = material.m_shininess_texture.valid;
	//			bool has_emission_texture = material.m_emission_texture.valid;
	//			if (has_color_texture)
	//			{
	//				glActiveTexture(GL_TEXTURE0);
	//				glBindTexture(GL_TEXTURE_2D, material.m_color_texture.gl_id);
	//			}
	//			// Actually unused in the labs
	//			/*
	//			if ( has_metalness_texture )
	//			{
	//				glActiveTexture( GL_TEXTURE2 );
	//				glBindTexture( GL_TEXTURE_2D, material.m_metalness_texture.gl_id );
	//			}
	//			if ( has_fresnel_texture )
	//			{
	//				glActiveTexture( GL_TEXTURE3 );
	//				glBindTexture( GL_TEXTURE_2D, material.m_fresnel_texture.gl_id );
	//			}
	//			if ( has_shininess_texture )
	//			{
	//				glActiveTexture( GL_TEXTURE4 );
	//				glBindTexture( GL_TEXTURE_2D, material.m_shininess_texture.gl_id );
	//			}
	//			*/
	//			if (has_emission_texture)
	//			{
	//				glActiveTexture(GL_TEXTURE5);
	//				glBindTexture(GL_TEXTURE_2D, material.m_emission_texture.gl_id);
	//			}
	//			glActiveTexture(GL_TEXTURE0);
	//
	//			// TODO: Set using shader::SetUniform*
	//			/*
	//			GLUtility::setUniformSlow(current_program, "has_color_texture", has_color_texture);
	//			GLUtility::setUniformSlow(current_program, "has_emission_texture", has_emission_texture);
	//			GLUtility::setUniformSlow(current_program, "material_color", material.m_color);
	//			GLUtility::setUniformSlow(current_program, "material_metalness", material.m_metalness);
	//			GLUtility::setUniformSlow(current_program, "material_fresnel", material.m_fresnel);
	//			GLUtility::setUniformSlow(current_program, "material_shininess", material.m_shininess);
	//			GLUtility::setUniformSlow(current_program, "material_emission", material.m_emission);
	//			*/
	//
	//			// Actually unused in the labs
	//			/*
	//			setUniformSlow( current_program, "has_metalness_texture", has_metalness_texture );
	//			setUniformSlow( current_program, "has_fresnel_texture", has_fresnel_texture );
	//			setUniformSlow( current_program, "has_shininess_texture", has_shininess_texture );
	//			*/
	//		}
	//		glDrawArrays(GL_TRIANGLES, mesh.m_start_index, (GLsizei)mesh.m_number_of_vertices);
	//	}
	//	glBindVertexArray(0);
	//}
	

}