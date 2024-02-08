#pragma once

#include "InputEvent.h"
#include "Game.h"
#include <unordered_set>
#include <memory>

namespace Shard {
	enum EventType {
		MouseMotion = 0,
		MouseDown = 1,
		MouseUp = 2,
		MouseWheel = 3,
		KeyDown = 4,
		KeyUp = 5
	};

	class InputListener{
	public:
		virtual ~InputListener() {}
		virtual void handleEvent(InputEvent ie, EventType et) = 0;
	};

	class InputManager {
	public:
		 void initialize();
		 void addListeners(std::shared_ptr<InputListener> listener);
		 void removeListeners(std::shared_ptr<InputListener> listener);
		 void informListeners(InputEvent ie, EventType et);
		 void getInput();
	private:
		std::vector<std::shared_ptr<InputListener>> myListeners;
		double tick{};
		double time_interval{};
	};
}
