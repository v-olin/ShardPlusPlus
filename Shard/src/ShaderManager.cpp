#include "ShaderManager.h"
#include "Logger.h"

#include "gtc/type_ptr.hpp"

#include <fstream>
#include <sstream>

namespace Shard {

	GLuint ShaderManager::loadShader(std::string vert_path, std::string frag_path, bool allow_errors) {
		GLuint v_shader = glCreateShader(GL_VERTEX_SHADER);
		GLuint f_shader = glCreateShader(GL_FRAGMENT_SHADER);

		std::ifstream vs_file(vert_path);
		std::string vs_src((std::istreambuf_iterator<char>(vs_file)), std::istreambuf_iterator<char>());

		std::ifstream fs_file(vert_path);
		std::string fs_src((std::istreambuf_iterator<char>(fs_file)), std::istreambuf_iterator<char>());

		//const char* vs = vs_src.c_str();
		//const char* fs = fs_src.c_str();
		const char* vs = R"(
    #version 420

    layout (location = 0) in vec3 position;
    layout (location = 1) in vec3 normal;
    layout (location = 2) in vec2 texCoord;

    uniform mat4 u_MVP;

    out vec3 normal_;
    out vec2 texCoord_;

    void main() {
        gl_Position = u_MVP * vec4(position, 1.0);
        normal_ = normal;
        texCoord_ = texCoord;
    }
)";
		const char*  fs = R"(
    #version 420

    in vec3 normal_;
    in vec2 texCoord_;

    uniform vec3 u_ObjectColor;

    out vec4 FragColor;

    void main() {
        FragColor = vec4(u_ObjectColor, 1.0);
    }
)";
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
			//std::string msg = "Could not compile vertex shader " + vert_path;
			std::string msg = getShaderInfoLog(v_shader);
			LoggerLevel lvl = allow_errors ? LOG_LEVEL_ERROR : LOG_LEVEL_FATAL;
			Logger::log(msg.c_str());
		}

		GLuint shader_program = glCreateProgram();
		glAttachShader(shader_program, v_shader);
		glDeleteShader(v_shader);
		glAttachShader(shader_program, f_shader);
		glDeleteShader(f_shader);

		/*
		if (!allow_errors)
			checkGLError(__FILE__, __LINE__) && (__debugbreak(), 1);
		*/

		if (!linkShaderProgram(shader_program, allow_errors))
			return 0;

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

	std::string ShaderManager::getShaderInfoLog(GLuint obj) {
		int log_len = 0;
		int chars_written = 0;
		char* tmp_log;
		std::string log;

		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &log_len);

		if (log_len > 0) {
			tmp_log = new char[log_len];
			glGetShaderInfoLog(obj, log_len, &chars_written, tmp_log);
			log = tmp_log;
			delete[] tmp_log;
		}

		return log;
	}

	std::string ShaderManager::getShaderProgramInfoLog(GLuint obj) {
		int log_len = 0;
		int chars_written = 0;
		char* tmp_log;
		std::string log;

		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &log_len);

		if (log_len > 0) {
			tmp_log = new char[log_len];
			glGetProgramInfoLog(obj, log_len, &chars_written, tmp_log);
			log = tmp_log;
			delete[] tmp_log;
		}

		return log;
	}

	void ShaderManager::SetMat4x4(const glm::mat4x4& mat, const std::string& uniform_name) {
		auto uniform_location = GetUniformLoc(uniform_name);
		glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(mat));
	}

	void ShaderManager::SetVec4(const glm::vec4& vec, const std::string& uniform_name) {
		auto uniform_location = GetUniformLoc(uniform_name);
		glUniform4fv(uniform_location, 1, glm::value_ptr(vec));
	}

	void ShaderManager::SetVec3(const glm::vec3& vec, const std::string& uniform_name)
	{
		auto uniform_location = GetUniformLoc(uniform_name);
		glUniform3fv(uniform_location, 1, glm::value_ptr(vec));
	}

	void ShaderManager::SetInteger1(const int& integer, const std::string& uniform_name)
	{
		auto uniform_location = GetUniformLoc(uniform_name);
		glUniform1i(uniform_location, integer);
	}

	void ShaderManager::SetFloat1(const float& float_, const std::string& uniform_name)
	{
		auto uniform_location = GetUniformLoc(uniform_name);
		glUniform1f(uniform_location, float_);
	}

	GLint ShaderManager::GetUniformLoc(const std::string& uniform_name)
	{
		// TODO: send in shader_id too otherwise can't use
		auto it = m_UniformCache.find(uniform_name);
		if (it == m_UniformCache.end()) {
			GLint loc = glGetUniformLocation(current_shader_id, uniform_name.c_str());
			m_UniformCache[uniform_name] = loc;
			assert(loc != -1); // uniform not found
			return loc;
		}

		return it->second;
	}

}

