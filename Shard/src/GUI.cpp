#include "GUI.h"
#include "Bootstrap.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "SceneManager.h"

namespace Shard {

	GUI::GUI(GLFWwindow* window)
		: m_window(window)
		, m_sceneManager(SceneManager::getInstance())
		, m_guiIsFocused(false)
		, m_focusedModel(nullptr)
	{
		initialize();
	}

	void GUI::initialize() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(m_window, true);
		ImGui_ImplOpenGL3_Init("#version 420");
	}

	bool GUI::isFocused() {
		return m_guiIsFocused;
	}
	void GUI::addRadioSelector(RadioSelector selector) {
		radioselectors.push_back(selector);
	}
	void GUI::addCheckBox(const char * text, bool* value) {
		std::pair<const char *, bool*> pair = std::make_pair(text, value);
		checkboxes.push_back(pair);
	}
	void GUI::addIntSlider(const char * text, int* value, int min, int max) {
		std::tuple<const char *, int*, int, int> tuple = std::make_tuple(text, value, min, max);
		intSliders.push_back(tuple);

	}
	void GUI::addFloatSlider(const char * text, float* value, float min, float max) {
		std::tuple<const char *, float*, float, float> tuple = std::make_tuple(text, value, min, max);
		floatSliders.push_back(tuple);
	}


	void GUI::draw() {
		

		// potentially needed
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Menu")) {
				m_guiIsFocused = true;

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
					ImGui::GetIO().Framerate);

				for (auto rs : radioselectors) {
					ImGui::Text(rs.text);
					for (auto s : rs.selctables)
						ImGui::RadioButton(std::get<0>(s), rs.value, std::get<1>(s));
				}
				for (auto cb : checkboxes)
					ImGui::Checkbox(cb.first, cb.second);
				for (auto is : intSliders)
					ImGui::SliderInt(std::get<0>(is), std::get<1>(is), std::get<2>(is), std::get<3>(is));
				for (auto fs : floatSliders)
					ImGui::SliderFloat(std::get<0>(fs), std::get<1>(fs), std::get<2>(fs), std::get<3>(fs));

				ImGui::EndMenu();
			}
			else
				m_guiIsFocused = false;
		
			ImGui::EndMainMenuBar();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}