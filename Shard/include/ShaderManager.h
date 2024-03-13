#pragma once

#include <string>
#include <unordered_set>
#include <memory>

#include "common.h"

#include "glm.hpp"

#include <unordered_map>

namespace Shard {

	// this gets its own class such that the manager
	// can handle multiple independent shaders and their
	// respective caches
	class ShaderUniformCache {
	public:
		ShaderUniformCache(GLuint shader);
		GLint findUniformLocation(const std::string& name);

	private:
		std::unordered_map<std::string, GLint> m_uniformCache;
		GLuint m_shaderProgram; // owner of the uniform
	};

	class ShaderManager {
	public:
		static ShaderManager& getInstance() {
			static ShaderManager instance;
			return instance;
		}

		GLuint loadShader(std::string shader_name, bool allow_errors);
		GLuint loadShader(std::string shader_path, std::string shader_name, bool allow_errors);

		GLuint getShader(std::string shader_name) { 
			return m_Shaders.at(shader_name);  
		};

		void SetMat4x4(GLuint program, const glm::mat4x4& mat, const std::string& uniform_name);
		void SetVec4(GLuint program, const glm::vec4& vec, const std::string& uniform_name);
		void SetVec3(GLuint program, const glm::vec3& vec, const std::string& uniform_name);
		void SetInteger1(GLuint program, const int& integer, const std::string& uniform_name);
		void SetFloat1(GLuint program, const float& float_, const std::string& uniform_name);
		GLint GetUniformLoc(GLuint shaderProgram, const std::string& uniform_name);
		const GLuint getDefaultShader();

	private:
		ShaderManager();

		bool linkShaderProgram(GLuint shader_program, bool allow_errors);
		std::string getShaderInfoLog(GLuint shaderProgram);
		std::string getShaderProgramInfoLog(GLuint shaderProgram);

		GLuint m_defaultShader;
		const std::string m_shaderPath;
		std::unordered_map<GLuint, std::shared_ptr<ShaderUniformCache>> m_shaderCaches{};
		std::unordered_map<std::string, GLuint> m_Shaders{};
	};
}