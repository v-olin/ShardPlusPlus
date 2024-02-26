#include "FBO.h"

#include "Logger.h"
#include <cstdint>

namespace Shard {

	FBO::FBO()
		: isComplete(false)
		, frameBufferId(SIZE_MAX)
		, colorTextureTarget(SIZE_MAX)
		, depthBuffer(SIZE_MAX)
		, width(0)
		, height(0)
	{ }

	FBO::FBO(size_t w, size_t h) : FBO() {
		resize(w, h);
	}

	void FBO::resize(size_t w, size_t h) {
		width = w;
		height = h;

		if (colorTextureTarget == SIZE_MAX) {
			glGenTextures(1, &colorTextureTarget);
			glBindTexture(GL_TEXTURE_2D, colorTextureTarget);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		if (depthBuffer == SIZE_MAX) {
			glGenTextures(1, &depthBuffer);
			glBindTexture(GL_TEXTURE_2D, depthBuffer);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glBindTexture(GL_TEXTURE_2D, colorTextureTarget);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glBindTexture(GL_TEXTURE_2D, depthBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

		if (!isComplete) {
			glGenFramebuffers(1, &frameBufferId);
			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextureTarget, 0);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);

			isComplete = checkFrameBufferComplete();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}

	bool FBO::checkFrameBufferComplete() {
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (status != GL_FRAMEBUFFER_COMPLETE) {
			LoggerLevel lvl = LOG_LEVEL_FATAL;
#ifdef _DEBUG
			lvl = LOG_LEVEL_ERROR;
#endif
			Logger::log("Framebuffer not complete", lvl);
		}

		return status == GL_FRAMEBUFFER_COMPLETE;
	}
}