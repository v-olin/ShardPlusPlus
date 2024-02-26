#pragma once

#include <glad/glad.h>

namespace Shard {

	class FBO {
	public:
		GLuint frameBufferId, colorTextureTarget, depthBuffer;
		size_t width, height;
		bool isComplete;

		FBO();
		FBO(size_t w, size_t h);

		void resize(size_t w, size_t h);
		bool checkFrameBufferComplete();
	};


}