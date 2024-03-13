#pragma once

#include "InputEvent.h"
#include "Game.h"
#include <unordered_set>
#include <memory>
#include <queue>

#include "common.h"

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

		 static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
		 static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
		 static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		 static void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset);
		 static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);


		std::queue<std::pair<InputEvent, EventType>> event_queue;
	private:
		std::vector<std::shared_ptr<InputListener>> myListeners;
		double tick{0.0};
		double time_interval{1.0/50.0};
	};
}
