#include "ShaderManager.h"
#include "Logger.h"

#include "gtc/type_ptr.hpp"

#include <fstream>

namespace Shard {

	ShaderUniformCache::ShaderUniformCache(GLuint shader)
		: m_shaderProgram(shader) { }

	GLint ShaderUniformCache::findUniformLocation(const std::string& name) {
		auto it = m_uniformCache.find(name);
		if (it == m_uniformCache.end()) {
			GLint loc = glGetUniformLocation(m_shaderProgram, name.c_str());
			assert(loc != -1 && "Uniform not found, very bad!!");
			m_uniformCache[name] = loc;
			return loc;
		}

		return it->second;
	}

	ShaderManager::ShaderManager()
		: m_shaderPath("../Shard/res/shaders/")
	{
		m_defaultShader = loadShader("default", false);

		if (!m_defaultShader) {
			Logger::log("Could not load default shader, very bad!!", LOG_LEVEL_FATAL);
		}
	}

	const GLuint ShaderManager::getDefaultShader() {
		return m_defaultShader;
	}

	GLuint ShaderManager::loadShader(std::string shader_name, bool allow_errors) {

		std::string path_vertex_shader = m_shaderPath + shader_name + ".vert";
		std::string path_fragment_shader = m_shaderPath + shader_name + ".frag";

		std::ifstream vs_file(path_vertex_shader);
		std::string vs_src((std::istreambuf_iterator<char>(vs_file)), std::istreambuf_iterator<char>());

		std::ifstream fs_file(path_fragment_shader);
		std::string fs_src((std::istreambuf_iterator<char>(fs_file)), std::istreambuf_iterator<char>());

		const char* vs = vs_src.c_str();
		const char* fs = fs_src.c_str();

		GLuint v_shader = glCreateShader(GL_VERTEX_SHADER);
		GLuint f_shader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(v_shader, 1, &vs, nullptr);
		glShaderSource(f_shader, 1, &fs, nullptr);

		glCompileShader(v_shader);
		int compile_ok = 0;
		glGetShaderiv(v_shader, GL_COMPILE_STATUS, &compile_ok);

		if (!compile_ok) {
			//std::string msg = "Could not compile vertex shader " + vert_path;
			std::string msg = getShaderInfoLog(v_shader);
			LoggerLevel lvl = allow_errors ? LOG_LEVEL_ERROR : LOG_LEVEL_FATAL;
			Logger::log(msg.c_str());
		}

		glCompileShader(f_shader);
		compile_ok = 0;
		glGetShaderiv(f_shader, GL_COMPILE_STATUS, &compile_ok);

		if (!compile_ok) {
			std::string msg = getShaderInfoLog(v_shader);
			LoggerLevel lvl = allow_errors ? LOG_LEVEL_ERROR : LOG_LEVEL_FATAL;
			Logger::log(msg.c_str());
		}

		GLuint shader_program = glCreateProgram();
		glAttachShader(shader_program, v_shader);
		glDeleteShader(v_shader);
		glAttachShader(shader_program, f_shader);
		glDeleteShader(f_shader);


		if (!linkShaderProgram(shader_program, allow_errors))
			return 0;

		// create cache for newly loaded shader
		m_shaderCaches[shader_program] = std::make_unique<ShaderUniformCache>(shader_program);

		return shader_program;
	}

	bool ShaderManager::linkShaderProgram(GLuint shader_program, bool allow_errors) {
		glLinkProgram(shader_program);
		GLint link_ok = 0;
		glGetProgramiv(shader_program, GL_LINK_STATUS, &link_ok);

		if (!link_ok) {
			std::string err = getShaderProgramInfoLog(shader_program);
			Logger::log(("!link_ok: " + err).c_str());
			return false;
		}
		
		return true;
	}

	std::string ShaderManager::getShaderInfoLog(GLuint shaderProgram) {
		int log_len = 0;
		int chars_written = 0;
		char* tmp_log;
		std::string log;

		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &log_len);

		if (log_len > 0) {
			tmp_log = new char[log_len];
			glGetShaderInfoLog(shaderProgram, log_len, &chars_written, tmp_log);
			log = tmp_log;
			delete[] tmp_log;
		}

		return log;
	}

	std::string ShaderManager::getShaderProgramInfoLog(GLuint shaderProgram) {
		int log_len = 0;
		int chars_written = 0;
		char* tmp_log;
		std::string log;

		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &log_len);
		

		if (log_len > 0) {
			tmp_log = new char[log_len];
			glGetProgramInfoLog(shaderProgram, log_len, &chars_written, tmp_log);
			log = tmp_log;
			delete[] tmp_log;
		}

		return log;
	}

	void ShaderManager::SetMat4x4(GLuint shader, const glm::mat4x4& mat, const std::string& uniform_name) {
		GLint tmp;
		// this is not performant, but idgaf, it is necessary
		glGetIntegerv(GL_CURRENT_PROGRAM, &tmp);
		GLuint active = (GLuint)(tmp);
		assert(shader == active && "Selected shader is not active, very bad!!");
		auto uniform_location = GetUniformLoc(shader, uniform_name);
		glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(mat));
	}

	void ShaderManager::SetVec4(GLuint shader, const glm::vec4& vec, const std::string& uniform_name) {
		GLint tmp;
		// this is not performant, but idgaf, it is necessary
		glGetIntegerv(GL_CURRENT_PROGRAM, &tmp);
		GLuint active = (GLuint)(tmp);
		assert(shader == active && "Selected shader is not active, very bad!!");
		auto uniform_location = GetUniformLoc(shader, uniform_name);
		glUniform4fv(uniform_location, 1, glm::value_ptr(vec));
	}

	void ShaderManager::SetVec3(GLuint shader, const glm::vec3& vec, const std::string& uniform_name) {
		GLint tmp;
		// this is not performant, but idgaf, it is necessary
		glGetIntegerv(GL_CURRENT_PROGRAM, &tmp);
		GLuint active = (GLuint)(tmp);
		assert(shader == active && "Selected shader is not active, very bad!!");
		auto uniform_location = GetUniformLoc(shader, uniform_name);
		glUniform3fv(uniform_location, 1, glm::value_ptr(vec));
	}

	void ShaderManager::SetInteger1(GLuint shader, const int& integer, const std::string& uniform_name) {
		GLint tmp;
		// this is not performant, but idgaf, it is necessary
		glGetIntegerv(GL_CURRENT_PROGRAM, &tmp);
		GLuint active = (GLuint)(tmp);
		assert(shader == active && "Selected shader is not active, very bad!!");
		auto uniform_location = GetUniformLoc(shader, uniform_name);
		glUniform1i(uniform_location, integer);
	}

	void ShaderManager::SetFloat1(GLuint shader, const float& float_, const std::string& uniform_name) {
		GLint tmp;
		// this is not performant, but idgaf, it is necessary
		glGetIntegerv(GL_CURRENT_PROGRAM, &tmp);
		GLuint active = (GLuint)(tmp);
		assert(shader == active && "Selected shader is not active, very bad!!");
		auto uniform_location = GetUniformLoc(shader, uniform_name);
		glUniform1f(uniform_location, float_);
	}

	GLint ShaderManager::GetUniformLoc(GLuint shaderProgram, const std::string& uniform_name) {
		auto it = m_shaderCaches.find(shaderProgram);

		if (it == m_shaderCaches.end()) {
			Logger::log("Selected shader does not exist, very bad!!", LOG_LEVEL_ERROR);
			return INT32_MAX;
		}

		return it->second->findUniformLocation(uniform_name);
	}

}

