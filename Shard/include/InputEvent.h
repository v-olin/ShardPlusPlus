#pragma once
#include "PhysicsBody.h"
#include <string>
namespace Shard {
	struct  InputEvent {
    public:
        int x;
        int y;
        int button;
        int key;
        std::shared_ptr<PhysicsBody> body;
        std::string classification;
	};
}
