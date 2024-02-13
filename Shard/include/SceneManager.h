#pragma once

#include "GameObject.h"
#include "Camera.h"
#include <memory>
#include <vector>

class Camera;
class Model;

namespace Shard {

	class SceneManager {
	public:
		void Draw();
	private:
		Camera camera{};
	};

}