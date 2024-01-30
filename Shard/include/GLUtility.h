#pragma once

#include <GL/glew.h>
#include <glm.hpp>

namespace GLUtility {

	inline void setUniformSlow(GLuint shaderProgram, const char* name, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name), 1, false, &matrix[0].x);
	}

	inline void setUniformSlow(GLuint shaderProgram, const char* name, const float value)
	{
		glUniform1f(glGetUniformLocation(shaderProgram, name), value);
	}

	inline void setUniformSlow(GLuint shaderProgram, const char* name, const GLint value)
	{
		int loc = glGetUniformLocation(shaderProgram, name);
		glUniform1i(loc, value);
	}

	inline void setUniformSlow(GLuint shaderProgram, const char* name, const GLuint value)
	{
		int loc = glGetUniformLocation(shaderProgram, name);
		glUniform1ui(loc, value);
	}

	inline void setUniformSlow(GLuint shaderProgram, const char* name, const bool value)
	{
		int loc = glGetUniformLocation(shaderProgram, name);
		glUniform1i(loc, value ? 1 : 0);
	}

	inline void setUniformSlow(GLuint shaderProgram, const char* name, const glm::vec3& value)
	{
		glUniform3fv(glGetUniformLocation(shaderProgram, name), 1, &value.x);
	}

	inline void setUniformSlow(GLuint shaderProgram, const char* name, const uint32_t nof_values, const glm::vec3* values)
	{
		glUniform3fv(glGetUniformLocation(shaderProgram, name), nof_values, (float*)values);
	}
}
