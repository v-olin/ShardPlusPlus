#pragma once

#include "common.h"
#include "Model.h"
#include "SceneManager.h"
#include <memory>

namespace Shard {

	class GUI {
	public:
		GUI(GLFWwindow* window, SceneManager& sceneManager);

		void initialize();
		void draw();
		void addButton(std::string text);
		void addSlider();
		bool isFocused();
	private:
		GLFWwindow* m_window;
		SceneManager& m_sceneManager;
		bool m_guiIsFocused;

		std::shared_ptr<Model> m_focusedModel;
	};
}