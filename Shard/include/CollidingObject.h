#pragma once

#include <memory>
#include <string>
#include "PhysicsBody.h"

namespace Shard {
	class  CollidingObject {
	public:
		std::shared_ptr<PhysicsBody> a, b;

		CollidingObject() : a(nullptr), b(nullptr) {}
		CollidingObject(std::shared_ptr<PhysicsBody> a, std::shared_ptr<PhysicsBody> b) :a(a), b(b) {}

		bool operator==(const CollidingObject& other){
			return a->equals(other.a) && b->equals(other.b);
		}

		bool equals(const CollidingObject& other){
			return a == other.a && b == other.b;
		}
	};
}
