#pragma once

#include "common.h"
#include "Model.h"
#include "SceneManager.h"
#include <memory>

namespace Shard {


	struct RadioSelector {
		const char * text;
		int* value;
		std::vector<std::tuple<const char *, int>>selctables;
	};


	class GUI {
	public:
		GUI(GLFWwindow* window);

		void initialize();
		void draw();
		bool isFocused();
		void addRadioSelector(RadioSelector selector);
		void addCheckBox(const char * text, bool* value);
		void addIntSlider(const char * text, int* value, int min, int max);
		void addFloatSlider(const char * text, float* value, float min, float max);
	private:
		GLFWwindow* m_window;
		SceneManager& m_sceneManager;
		bool m_guiIsFocused;
		std::vector<const char *> texts;



		std::vector<RadioSelector> radioselectors;
		std::vector<std::pair<const char *, bool*>> checkboxes;
		std::vector<std::tuple<const char *, int*, int, int>> intSliders;
		std::vector<std::tuple<const char *, float*, float, float>> floatSliders;

		std::shared_ptr<Model> m_focusedModel;
	};


}