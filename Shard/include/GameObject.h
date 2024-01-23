#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include "CollisionHandler.h"

namespace Shard {
    class SHARD_API GameObject : CollisionHandler {

    };
}