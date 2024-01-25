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

		Transform3D() : Transform() {
			
			z = 0;
			rotx = 0;
			roty = 0;
			scale_z = 1.f;
		}

	private:
		double z, rotx, roty;
		float scale_z;
	};
}