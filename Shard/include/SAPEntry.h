#pragma once

#include "PhysicsBody.h"

namespace Shard {
	struct  SAPEntry {
	public:
		std::shared_ptr<PhysicsBody> owner;
		float start, end;
		//SAPEntry &prev, &next; // use std::list idiot
		
		bool operator==(const SAPEntry& other) {
			return (start == other.start) && (end == other.end) && (owner == other.owner);
		}

		bool operator < (const SAPEntry& other) {
			return start < other.start;
		}

		static int compareSAP(SAPEntry& a, SAPEntry& b) {
			return a.start - b.start;
		}
	};
}