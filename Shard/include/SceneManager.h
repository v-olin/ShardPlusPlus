#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include "TextureManager.h"
#include "Camera.h"

namespace Shard {
	class SceneManager {
	public:


	private:
		Camera camera;
		TextureManager& textureManager;

	};
}