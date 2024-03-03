#pragma once

#include <glad/glad.h>

namespace Shard {

	class RenderableObject {
	public:
		virtual void render() = 0;
	};
}