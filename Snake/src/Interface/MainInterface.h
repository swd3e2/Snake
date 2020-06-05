#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <entt/entt.hpp>
#include "../Model/Model.h"

class MainInterface {
private:
	const char* glsl_version = "#version 130";
	entt::registry* registry;
	entt::entity selectedEntity;
	bool isEntitySelected = false;
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
		std::string temp = "Entity №";
		int i = 0;
		registry->each([&](const entt::entity& entity) {
			if (ImGui::Selectable(("Entity###" + std::to_string(i)).c_str())) {
				selectedEntity = entity;
				isEntitySelected = true;
			}
			i++;
		});
		ImGui::End();

		ImGui::Begin("Entities");
		ImGui::Text(("Delta " + std::to_string(dt)).c_str());
		ImGui::Text("Nodes");

		if (isEntitySelected && registry->has<Render>(selectedEntity)) {
			const Render& render = registry->get<Render>(selectedEntity);
			if (render.model != nullptr) {
				drawNodeHierarchy(render.model->getRootNode(), render.model->getNodes());
			}

			const Transform& transform = registry->get<Transform>(selectedEntity);
			ImGui::DragFloat3("Translation###T", (float*)&transform.translation, 0.01f, -10000.0f, 10000.0f);
			ImGui::DragFloat3("Scale###S", (float*)&transform.scale, 0.01f, -10000.0f, 10000.0f);
			ImGui::DragFloat3("Rotation###R", (float*)&transform.rotation, 0.01f, -10000.0f, 10000.0f);
		}
		
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void drawNodeHierarchy(int nodeId, const std::vector<std::shared_ptr<Model::Node>>& nodes)
	{
		const std::shared_ptr<Model::Node>& node = nodes[nodeId];
		if (ImGui::TreeNode(node->name.c_str())) {
			ImGui::DragFloat3(("Translation###NT" + node->name).c_str(), (float*)&node->transform.translation, 0.01f, -10000.0f, 10000.0f);
			ImGui::DragFloat3(("Scale###NS" + node->name).c_str(), (float*)&node->transform.scale, 0.01f, -10000.0f, 10000.0f);
			ImGui::DragFloat4(("Rotation###NR" + node->name).c_str(), (float*)&node->transform.rotation, 0.01f, -10000.0f, 10000.0f);

			char str[80];

			ImGui::Text("Transform matrix");
			sprintf(str, "%f %f %f %f", node->transform.worldTransform[0][0], node->transform.worldTransform[0][1], node->transform.worldTransform[0][2], node->transform.worldTransform[0][3]);
			ImGui::Text(str);
			sprintf(str, "%f %f %f %f", node->transform.worldTransform[1][0], node->transform.worldTransform[1][1], node->transform.worldTransform[1][2], node->transform.worldTransform[1][3]);
			ImGui::Text(str);
			sprintf(str, "%f %f %f %f", node->transform.worldTransform[2][0], node->transform.worldTransform[2][1], node->transform.worldTransform[2][2], node->transform.worldTransform[2][3]);
			ImGui::Text(str);
			sprintf(str, "%f %f %f %f", node->transform.worldTransform[3][0], node->transform.worldTransform[3][1], node->transform.worldTransform[3][2], node->transform.worldTransform[3][3]);
			ImGui::Text(str);
			
			ImGui::Text("Normal matrix");
			sprintf(str, "%f %f %f %f", node->transform.matrixTransform[0][0], node->transform.matrixTransform[0][1], node->transform.matrixTransform[0][2], node->transform.matrixTransform[0][3]);
			ImGui::Text(str);
			sprintf(str, "%f %f %f %f", node->transform.matrixTransform[1][0], node->transform.matrixTransform[1][1], node->transform.matrixTransform[1][2], node->transform.matrixTransform[1][3]);
			ImGui::Text(str);
			sprintf(str, "%f %f %f %f", node->transform.matrixTransform[2][0], node->transform.matrixTransform[2][1], node->transform.matrixTransform[2][2], node->transform.matrixTransform[2][3]);
			ImGui::Text(str);
			sprintf(str, "%f %f %f %f", node->transform.matrixTransform[3][0], node->transform.matrixTransform[3][1], node->transform.matrixTransform[3][2], node->transform.matrixTransform[3][3]);
			ImGui::Text(str);

			for (auto& it : node->childs) {
				drawNodeHierarchy(it, nodes);
			}

			ImGui::TreePop();
		}
	}
};