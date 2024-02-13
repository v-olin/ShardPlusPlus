#pragma once

#include <string>
#include <unordered_set>

#include "common.h"

#include "glm.hpp"

#include <unordered_map>

namespace Shard {
	class ShaderManager {
	public:

		GLuint loadShader(std::string vert_path, std::string frag_path, bool allow_errors);

		GLuint getShader(std::string shader_name) { 
			return m_Shaders[shader_name];  
		};

		void SetMat4x4(const glm::mat4x4& mat, const std::string& uniform_name);
		void SetVec4(const glm::vec4& vec, const std::string& uniform_name);
		void SetVec3(const glm::vec3& vec, const std::string& uniform_name);
		void SetInteger1(const int& integer, const std::string& uniform_name);
		void SetFloat1(const float& float_, const std::string& uniform_name);
		GLint GetUniformLoc(const std::string& uniform_name);

		GLuint current_shader_id;
	private:
		bool linkShaderProgram(GLuint shader_program, bool allow_errors);
		std::string getShaderInfoLog(GLuint obj);
		std::string getShaderProgramInfoLog(GLuint obj);

		std::unordered_map<std::string, GLint> m_UniformCache{};


		// probably not necessary
		std::unordered_map<std::string, GLuint> m_Shaders{};
		//std::unordered_set<GLuint> _shaders;
	};
}