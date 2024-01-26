#include "ShaderManager.h"
#include "Logger.h"

#include <fstream>
#include <sstream>
#include <debugapi.h>

namespace Shard {

	GLuint ShaderManager::loadShader(const std::string& vert_path, const std::string& frag_path, bool allow_errors) {
		GLuint v_shader = glCreateShader(GL_VERTEX_SHADER);
		GLuint f_shader = glCreateShader(GL_VERTEX_SHADER);

		std::ifstream vs_file(vert_path);
		std::string vs_src((std::istreambuf_iterator<char>(vs_file)), std::istreambuf_iterator<char>());

		std::ifstream fs_file(vert_path);
		std::string fs_src((std::istreambuf_iterator<char>(fs_file)), std::istreambuf_iterator<char>());

		const char* vs = vs_src.c_str();
		const char* fs = fs_src.c_str();

		glShaderSource(v_shader, 1, &vs, nullptr);
		glShaderSource(f_shader, 1, &fs, nullptr);

		glCompileShader(v_shader);
		int compile_ok = 0;
		glGetShaderiv(v_shader, GL_COMPILE_STATUS, &compile_ok);

		if (!compile_ok) {
			//std::string msg = "Could not compile vertex shader " + vert_path;
			std::string msg = getShaderInfoLog(v_shader);
			LoggerLevel lvl = allow_errors ? LOG_LEVEL_ERROR : LOG_LEVEL_FATAL;
			Logger::log(msg.c_str(), lvl);
		}

		glCompileShader(f_shader);
		compile_ok = 0;
		glGetShaderiv(f_shader, GL_COMPILE_STATUS, &compile_ok);

		if (!compile_ok) {
			//std::string msg = "Could not compile vertex shader " + vert_path;
			std::string msg = getShaderInfoLog(v_shader);
			LoggerLevel lvl = allow_errors ? LOG_LEVEL_ERROR : LOG_LEVEL_FATAL;
			Logger::log(msg.c_str(), lvl);
		}

		GLuint shader_program = glCreateProgram();
		glAttachShader(shader_program, v_shader);
		glDeleteShader(v_shader);
		glAttachShader(shader_program, f_shader);
		glDeleteShader(f_shader);

		if (!allow_errors)
			checkGLError(__FILE__, __LINE__) && (__debugbreak(), 1);

		if (!linkShaderProgram(shader_program, allow_errors))
			return 0;

		return shader_program;
	}

	bool ShaderManager::checkGLError(const char* file, int line) {
		bool wasError = false;

		for (GLenum glErr = glGetError(); glErr != GL_NO_ERROR; glErr = glGetError())
		{
			wasError = true;
			const GLubyte* sError = gluErrorString(glErr);

			if (!sError)
			{
				sError = reinterpret_cast<const GLubyte*>(" (no message available)");
			}

			std::cerr << "GL Error #" << glErr << "(" << sError << ") "
				<< " in File " << file << " at line: " << line << std::endl;

#if defined(_WIN32)
			std::stringstream ss;
			ss << file << "(" << line << "): GL Error #" << glErr << "(" << sError << ") " << std::endl;

			// outputs error message to debug console, if a debugger is attached.
			OutputDebugStringA(ss.str().c_str());
#endif
		}
		return wasError;
	}

	bool ShaderManager::linkShaderProgram(GLuint shader_program, bool allow_errors) {
		glLinkProgram(shader_program);
		GLint link_ok = 0;
		glGetProgramiv(shader_program, GL_LINK_STATUS, &link_ok);

		if (!link_ok) {
			std::string err = getShaderProgramInfoLog(shader_program);
			LoggerLevel lvl = allow_errors ? LOG_LEVEL_ERROR : LOG_LEVEL_FATAL;
			Logger::log(err, lvl);
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
}