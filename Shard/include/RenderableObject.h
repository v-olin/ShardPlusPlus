#pragma once

#include <glad/glad.h>

namespace Shard {

	class RenderableObject {
	public:
		// render
		virtual void render() = 0;
	};
}