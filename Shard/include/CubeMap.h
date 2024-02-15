#pragma once

#include "glad/glad.h"

namespace Shard {

	class CubeMap {
	public:
		CubeMap();
		~CubeMap();
		void Draw();
	private:
		GLuint m_VAO{0};
		GLuint m_VBO{0};
	};

} // end namespace
