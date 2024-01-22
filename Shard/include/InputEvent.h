#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <string>
namespace Shard {
	struct SHARD_API InputEvent {
    public:
        int x;
        int y;
        int button;
        int key;
        std::string classification;
	};
}
