#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include "PhysicsBody.h"

namespace Shard {
	struct SHARD_API SAPEntry {
	public:
		PhysicsBody* owner;
		float start, end;
		//SAPEntry &prev, &next; // use std::list idiot
		
		bool operator==(const SAPEntry& other) {
			return (start == other.start) && (end == other.end) && (owner == other.owner);
		}

	private:
	};
}