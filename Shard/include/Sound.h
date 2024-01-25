#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <string>

namespace Shard {
	class Sound {
	public:
		SHARD_API void playSound(std::string file_name);
	};
}