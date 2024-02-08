#pragma once

#include "Transform.h"

namespace Shard {
	class Transform3D : public Transform {
	public:

		Transform3D() : Transform(), z(0), rotx(0), roty(y), scale_z(1.0f) { }
		Transform3D(std::shared_ptr<Transform3D> src) : Transform(src) {
			this->z = src->z;
			this->rotx = src->rotx;
			this->roty = src->roty;
			this->scale_z = src->scale_z;
		}

	private:
		double z, rotx, roty;
		float scale_z;
	};
}