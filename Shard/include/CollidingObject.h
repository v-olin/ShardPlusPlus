#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <string>
#include "PhysicsBody.h"

namespace Shard {
	class SHARD_API CollidingObject {
	public:
		PhysicsBody a, b;

		CollidingObject();
		CollidingObject(PhysicsBody a, PhysicsBody b) {
			this->a = a;
			this->b = b;
		}

		bool operator==(CollidingObject other){
			return a.equals(other.a) && b.equals(other.b);
			//return (a == other.a) && (b == other.b);

		}
		bool equals(CollidingObject other){
			return a.equals(other.a) && b.equals(other.b);
		}


		// i think these are unneccessary // hindsight: it was :( 
		// bool equals(CollidingObject other);
		// int getHashcode();
		// std::string toString();
	};

}

template <>
struct std::hash<Shard::CollidingObject> {
	size_t operator()(const Shard::CollidingObject& obj) const {
		return 1;
		//return a.GetHashCode() ^ b.GetHashCode();
	}
};