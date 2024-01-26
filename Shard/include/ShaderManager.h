#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <string>
#include <unordered_set>

#include <GL/glew.h>

namespace Shard {
	class ShaderManager {
	public:

		GLuint loadShader(const std::string& vert_path, const std::string& frag_path, bool allow_errors);

	private:
		bool checkGLError(const char* file, int line);
		bool linkShaderProgram(GLuint shader_program, bool allow_errors);
		std::string getShaderInfoLog(GLuint obj);
		std::string getShaderProgramInfoLog(GLuint obj);

		// probably not necessary
		// std::unordered_set<GLuint> _shaders;
	};
}