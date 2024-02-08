#include <SDL_events.h>
#include "Bootstrap.h"
#include "InputManager.h"
#include "Logger.h"

namespace Shard {

	void InputManager::addListeners(std::shared_ptr<InputListener> listener) {
		myListeners.push_back(listener);
	}
	
	void InputManager::removeListeners(std::shared_ptr<InputListener> listener) {
		std::erase(myListeners, listener);
	}

	void InputManager::getInput() {
		SDL_Event ev;
		InputEvent ie;

		tick += Bootstrap::getDeltaTime();
		if (tick < time_interval)
			return;

		while (tick >= time_interval && SDL_PollEvent(&ev)) {

			if (ev.type == SDL_MOUSEMOTION) {
				SDL_MouseMotionEvent mot = ev.motion;
				ie.x = mot.x;
				ie.y = mot.y;
				informListeners(ie, MouseMotion);
			}
			if (ev.type == SDL_MOUSEBUTTONUP || ev.type == SDL_MOUSEBUTTONDOWN) {
				SDL_MouseButtonEvent butt = ev.button;
				ie.button = (int)butt.button;
				ie.x = butt.x;
				ie.y = butt.y;
				EventType type = ev.type == SDL_MOUSEBUTTONDOWN ? MouseDown : MouseUp;
				informListeners(ie, type);
			}
			if (ev.type == SDL_MOUSEWHEEL) {
				SDL_MouseWheelEvent wh = ev.wheel;
				ie.x = (int)wh.direction * wh.x;
				ie.y = (int)wh.direction * wh.y;
				informListeners(ie, MouseWheel);
			}
			if (ev.type == SDL_KEYDOWN || ev.type == SDL_KEYUP) {
				ie.key = (int)ev.key.keysym.scancode;
				EventType type = ev.type == SDL_KEYDOWN ? KeyDown : KeyUp;
				informListeners(ie, type);
			}
			tick -= time_interval;
		}
	}

	void InputManager::informListeners(InputEvent ie, EventType et) {
		int idx = 0;
		while (idx < myListeners.size()) {
			myListeners[idx++]->handleEvent(ie, et);
		}
	}

	void InputManager::initialize() {
		tick = 0;
		time_interval = 1.0 / 60.0;
	}

}