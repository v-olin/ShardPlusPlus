#pragma once

#include <string>
namespace Shard {
	struct  InputEvent {
    public:
        int x;
        int y;
        int button;
        int key;
        std::string classification;
	};
}
