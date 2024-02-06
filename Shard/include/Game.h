#pragma once

#include "AssetManager.h"

namespace Shard {

	class Game {
	public:
		virtual void initalize() = 0;
		virtual void update() = 0;
		virtual bool isRunning() {
			return true;
		}

		virtual int getTargetFrameRate() {
			return INT32_MAX;
		}
	};
}