#pragma once


#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif
#include "InputEvent.h"
#include <unordered_set>

namespace Shard {
	enum EventType {
			MouseMotion = 0,
			MouseDown = 1,
			MouseUp = 2,
			MouseWheel = 3,
			KeyDown = 4,
			KeyUp = 5
		};
	typedef void (*InputHandler)(InputEvent, EventType);
	class InputManager {
	public:
		


		SHARD_API void initialize();
		SHARD_API void addListeners(InputHandler);
		SHARD_API void removeListeners(InputHandler);
		SHARD_API void informListeners(InputEvent, EventType);
		SHARD_API void getInput();
	private:
		std::unordered_set<InputHandler> myListeners;
		double tick;

		double time_interval;

	};
}
