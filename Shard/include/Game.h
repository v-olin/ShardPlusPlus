#pragma once

#include "AssetManager.h"

namespace Shard {

	class Game {
	public:
		AssetManager* assets{ nullptr };

		AssetManager* getAssetManager() {
			if (!assets)
				assets = &AssetManager::getInstance();
			return assets;
		}
		virtual void initalize() {}
		virtual void update() {}
		virtual bool isRunning() {
			return true;
		}

		virtual int getTargetFrameRate() {
			return INT32_MAX;
		}
	};
}