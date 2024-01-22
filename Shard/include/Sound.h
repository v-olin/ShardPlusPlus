#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <string>

namespace Shard {
	class SHARD_API Sound {
	public:
		void playSound(std::string file_name);
	};
}