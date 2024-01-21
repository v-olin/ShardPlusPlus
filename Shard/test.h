// test.h - Description
#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

namespace Shard {

	extern "C" SHARD_API void PrintMessage();

}
