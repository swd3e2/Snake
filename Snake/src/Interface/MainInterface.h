#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <entt/entt.hpp>

class MainInterface {
private:
	const char* glsl_version = "#version 130";
	entt::registry* registry;
public:
	MainInterface(GLFWwindow* window, entt::registry* registry) :
		registry(registry)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
	}

	~MainInterface() {
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	
	void update(double dt) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Entities");

		std::string temp = "Entity ¹";
		registry->each([&temp](const entt::entity& entity) {
			ImGui::Text((temp + std::to_string((int)entity)).c_str());
		});
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
};