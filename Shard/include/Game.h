#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include "AssetManager.h"

namespace Shard {

	class Game {
	public:
		AssetManager* assets;

		SHARD_API AssetManager* getAssetManager() {
			assets = &AssetManager::getInstance();
			return assets;
		}
		virtual SHARD_API void initalize() {}
		virtual SHARD_API void update() {}
		virtual SHARD_API bool isRunning() {
			return true;
		}

		virtual SHARD_API int getTargetFrameRate() {
			return INT32_MAX;
		}
	};
}