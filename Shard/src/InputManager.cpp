#include "Bootstrap.h"
#include "InputManager.h"
#include "Logger.h"

#include "GLFW/glfw3.h"

namespace Shard {

	void InputManager::addListeners(std::shared_ptr<InputListener> listener) {
		myListeners.push_back(listener);
	}
	
	void InputManager::removeListeners(std::shared_ptr<InputListener> listener) {
		std::erase(myListeners, listener);
	}

	void InputManager::getInput() {
		
		tick += Bootstrap::getDeltaTime();
		if (tick < time_interval)
			return;
	
		while (!event_queue.empty() && tick >= time_interval) {
			auto &[ie, ev] = event_queue.front();
			informListeners(ie, ev);
			event_queue.pop();
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

	void InputManager::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
		InputManager* input_manager = reinterpret_cast<InputManager*>(glfwGetWindowUserPointer(window));
		// TODO: When glfw window is resized, potentially need to send
		// event to somewhere about the resize?
		// 
		// store new width and height somewhere lol, hardcoded atm..............................
		glViewport(0, 0, width, height);
	}

	void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		InputManager* input_manager = reinterpret_cast<InputManager*>(glfwGetWindowUserPointer(window));
		
		InputEvent ie;
		ie.key = key;
		EventType type = action == GLFW_PRESS ? KeyDown : KeyUp;
		input_manager->event_queue.push({ ie, type });
		//input_manager->informListeners(ie, type);

	}

	void InputManager::ScrollCallback(GLFWwindow* window, double x_offset, double y_offset)
	{
		InputManager* input_manager = reinterpret_cast<InputManager*>(glfwGetWindowUserPointer(window));
		// x always 0
		// y -1 if scroll down
		// y  1 if scroll up
		InputEvent ie;
		ie.x = x_offset;
		ie.y = y_offset,
		input_manager->event_queue.push({ ie, EventType::MouseWheel });
		//input_manager->informListeners(ie, EventType::MouseWheel);
	}

	void InputManager::MouseCallback(GLFWwindow* window, double xpos, double ypos)
	{
		InputManager* input_manager = reinterpret_cast<InputManager*>(glfwGetWindowUserPointer(window));

		InputEvent ie;
		ie.x = xpos;
		ie.y = ypos;
		input_manager->event_queue.push({ ie, EventType::MouseMotion });
		//input_manager->informListeners(ie, EventType::MouseMotion);
	}

	void InputManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{

		InputManager* input_manager = reinterpret_cast<InputManager*>(glfwGetWindowUserPointer(window));

		InputEvent ie;
		ie.button = button;

		double x, y;
		glfwGetCursorPos(window, &x, &y);
		ie.x = x;
		ie.y = y;

		EventType ev = action == GLFW_PRESS ? MouseDown : MouseUp;
		input_manager->event_queue.push({ ie, ev });
		//input_manager->informListeners(ie, ev);
	}


}