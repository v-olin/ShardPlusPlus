#include <SDL_events.h>
#include "Bootstrap.h"
#include "InputManager.h"

//This replaces all thigs regardnig input in the C# code.
//Except the InputBasic class, sice that class is never used in the original code...

namespace Shard {

	void InputManager::addListeners(InputListener* listener) {
		myListeners.insert(listener);
	}
	void InputManager::removeListeners(InputListener* listener) {
		myListeners.erase(listener);
	}

	void InputManager::getInput() {
		SDL_Event ev;
		InputEvent ie;

		//TODO add back when bootstrap is implemented
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
				//TODO fix when debug is implemented
				//Debug.getInstance().log("Keydown: " + ie.Key);
				EventType type = ev.type == SDL_KEYDOWN ? KeyDown : KeyUp;
				informListeners(ie, type);
			}
			tick -= time_interval;
		}
	}

	void InputManager::informListeners(InputEvent ie, EventType et) {
		auto s = myListeners.size();
		for (InputListener* listener : myListeners)
			listener->handleEvent(ie, et);
	}

	void InputManager::initialize() {
		tick = 0;
		time_interval = 1.0 / 60.0;
	}

}