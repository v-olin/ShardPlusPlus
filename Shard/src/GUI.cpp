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

	}

	bool GUI::isFocused() {
		return m_guiIsFocused;
	}

	void GUI::draw() {
		
		ImGui::Begin("");
		std::string second_fps = std::to_string(Shard::Bootstrap::getSecondFPS());
		std::string fps = std::to_string(Shard::Bootstrap::getFPS());

		// potentially needed
		//ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Camera")) {
				if (ImGui::MenuItem("Change FOV")) {
					ImGui::SliderFloat("FOV", &m_sceneManager.camera.fov, 20.f, 80.f);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Rendering")) {
				if (ImGui::MenuItem("Change render distance")) {
					ImGui::SliderFloat("Near plane", &m_renderer.m_nearPlane, 1.f, 100.f);
					ImGui::SliderFloat("Far plane", &m_renderer.m_farPlane, 100.f, 1000.f);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("FPS")) {
				if (ImGui::MenuItem("Show FPS")) {
					ImGui::Text("FPS: %.1f", second_fps + " / " + fps);
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		ImGui::Render();
	}
}