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

		void recalculateCentre() {
			Transform::recalculateCentre();
		}

		void translate(float x, float y) {
			Transform::translate(x, y);
		}

		void translate(glm::vec2 vect) {
			Transform::translate(vect);
		}

		void rotate(float dir) {
			Transform::rotate(dir);
		}

		glm::vec2 getLastDirection() {
			return Transform::getLastDirection();
		}

	private:
		double z, rotx, roty;
		float scale_z;
	};
}