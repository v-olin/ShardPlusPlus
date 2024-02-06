#pragma once

#include <string>
#include "PhysicsBody.h"

namespace Shard {
	class  CollidingObject {
	public:
		PhysicsBody* a;
		PhysicsBody* b;

		CollidingObject() {}
		CollidingObject(PhysicsBody* a, PhysicsBody* b) :a(a), b(b) {
		}

		bool operator==(CollidingObject other){
			return a->equals(other.a) && b->equals(other.b);
		}

		bool equals(CollidingObject other){
			return a == other.a && b == other.b;
			//return a.equals(other.a) && b.equals(other.b);
		}

		// i think these are unneccessary // hindsight: it was :( 
		// bool equals(CollidingObject other);
		// int getHashcode();
		// std::string toString();
	};
}
