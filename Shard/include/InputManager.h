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
	class SHARD_API InputManager {
	public:
		


		void initialize();
		void addListeners(InputHandler);
		void removeListeners(InputHandler);
		void informListeners(InputEvent, EventType);
		void getInput();
	private:
		std::unordered_set<InputHandler> myListeners;
		double tick;

		double time_interval;

	};
}
