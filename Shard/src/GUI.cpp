#include "GUI.h"
#include "Bootstrap.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace Shard {

	GUI::GUI(GLFWwindow* window, SceneManager& sceneManager, Renderer& renderer)
		: m_window(window)
		, m_sceneManager(sceneManager)
		, m_renderer(renderer)
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

	void GUI::draw() {
		
		std::string second_fps = std::to_string(Shard::Bootstrap::getSecondFPS());
		std::string fps = std::to_string(Shard::Bootstrap::getFPS());

		// potentially needed
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Camera")) {
				ImGui::SliderFloat("FOV", &m_sceneManager.camera.fov, 20.f, 80.f);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Rendering")) {
				ImGui::SliderFloat("Near plane", &m_renderer.m_nearPlane, 1.f, 100.f);
				ImGui::SliderFloat("Far plane", &m_renderer.m_farPlane, 100.f, 1000.f);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("FPS")) {
				ImGui::Text("FPS: %.1f", second_fps + " / " + fps);
				ImGui::EndMenu();
			}
		
			ImGui::EndMainMenuBar();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}