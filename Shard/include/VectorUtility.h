#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <functional>
#include <glm.hpp>

#define NULL_VECTOR glm::vec2{ std::numeric_limits<float>::min(), std::numeric_limits<float>::min()  };

namespace Shard {
	struct SHARD_API Vec2Comparator {
		bool operator()(glm::vec2 v1, glm::vec2 v2) {
			return v1.x == v2.x && v1.y == v2.y;
		}
	};
}