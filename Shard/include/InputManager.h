#pragma once

#include "InputEvent.h"
#include "Game.h"
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
	//typedef void (*InputHandler)(InputEvent, EventType);

	class InputListener {
	public:
		virtual  ~InputListener() {}
		virtual  void handleEvent(InputEvent ie, EventType et) = 0;
	};

	class InputManager {
	public:
		 void initialize();
		 void addListeners(InputListener*);
		 void removeListeners(InputListener*);
		 void informListeners(InputEvent, EventType);
		 void getInput();
	private:
		std::unordered_set<InputListener*> myListeners;
		double tick;
		double time_interval;
	};
}
