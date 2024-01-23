#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include "Transform.h"

namespace Shard {
	class SHARD_API Transform3D : public Transform {
	public:
		Transform3D(GameObject* game_obj);

	private:
		double z, rotx, roty;
		int scale_z;
	};
}