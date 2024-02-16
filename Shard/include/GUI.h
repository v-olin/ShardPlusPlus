#pragma once

#include "SceneManager.h"
#include "Renderer.h"
#include "Model.h"

#include <memory>

class GLFWwindow;

namespace Shard {

	class GUI {
	public:
		GUI(GLFWwindow* window, SceneManager& sceneManager, Renderer& renderer);

		void draw();
		void addButton(std::string text);
		void addSlider();
		bool isFocused();
	private:
		GLFWwindow* m_window;
		SceneManager& m_sceneManager;
		Renderer& m_renderer;
		bool m_guiIsFocused;

		std::shared_ptr<Model> m_focusedModel;
	};
}