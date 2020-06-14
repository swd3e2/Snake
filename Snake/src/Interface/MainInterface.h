#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <entt/entt.hpp>
#include "../Model/Model.h"
#include <ImGuizmo.h>
#include "Camera.h"
#include "Graphics/Platform/OpenGL/OpenGLRenderTarget.h"
#include "Saver.h"
#include "Model/Import/GltfImporter.h"
#include "RenderSystem.h"

class MainInterface {
private:
	const char* glsl_version = "#version 130";
	entt::registry* registry;
	entt::entity selectedEntity;
	bool isEntitySelected = false;
	Camera* camera;
	RenderSystem* renderSystem;
public:
	MainInterface(GLFWwindow* window, entt::registry* registry, Camera* camera, RenderSystem* renderSystem) :
		registry(registry), camera(camera), renderSystem(renderSystem)
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
		ImGuizmo::BeginFrame();

		ImGui::Begin("Entities");
		std::string temp = "Entity №";
		int i = 0;
		if (ImGui::Button("Create entity")) {
			isEntitySelected = true;
			selectedEntity = registry->create();
		}
		if (ImGui::Button("Save")) {
			Saver saver;
			saver.saveToFile("test.json", registry);
		}
		registry->each([&](const entt::entity& entity) {
			if (ImGui::Selectable(("Entity###" + std::to_string(i)).c_str())) {
				selectedEntity = entity;
				isEntitySelected = true;
			}
			i++;
		});
		//ImGui::Image((void*)std::static_pointer_cast<OpenGLTexture2D>(((OpenGLRenderTarget*)renderSystem->rt.get())->colorTexturesMap[0])->textureId, ImVec2(256, 144), ImVec2(0, 1), ImVec2(1, 0));
		//ImGui::Image((void*)std::static_pointer_cast<OpenGLTexture2D>(((OpenGLRenderTarget*)renderSystem->rt.get())->colorTexturesMap[1])->textureId, ImVec2(256, 144), ImVec2(0, 1), ImVec2(1, 0));
		//ImGui::Image((void*)std::static_pointer_cast<OpenGLTexture2D>(((OpenGLRenderTarget*)renderSystem->rt.get())->colorTexturesMap[2])->textureId, ImVec2(256, 144), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::Image((void*)std::static_pointer_cast<OpenGLTexture2D>(renderSystem->textures["bluredShadowDepthTexture"])->textureId, ImVec2(256, 144), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::Image((void*)std::static_pointer_cast<OpenGLTexture2D>(renderSystem->textures["shadowColorTexture"])->textureId, ImVec2(256, 144), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::Image((void*)std::static_pointer_cast<OpenGLTexture2D>(renderSystem->textures["rtt"])->textureId, ImVec2(256, 144), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::Image((void*)std::static_pointer_cast<OpenGLTexture2D>(renderSystem->textures["shadowDepthTexture"])->textureId, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();

		ImGui::Begin("Entities");
		ImGui::Text(("Delta " + std::to_string(dt)).c_str());
		ImGui::Text("Nodes");

		if (isEntitySelected) {
			if (registry->has<Render>(selectedEntity)) {
				const Render& render = registry->get<Render>(selectedEntity);
				if (render.model != nullptr) {
					drawNodeHierarchy(render.model->getRootNode(), render.model->getNodes());
				}

				Transform& transform = registry->get<Transform>(selectedEntity);
				ImGui::DragFloat3("Translation###T", (float*)&transform.translation, 0.01f, -10000.0f, 10000.0f);
				ImGui::DragFloat3("Scale###S", (float*)&transform.scale, 0.01f, -10000.0f, 10000.0f);
				ImGui::DragFloat3("Rotation###R", (float*)&transform.rotation, 0.01f, -10000.0f, 10000.0f);
				EditTransform(transform);
			} else {
				if (ImGui::Button("Create render component")) {
					GltfImporter importer;
					std::shared_ptr<Import::Model> model = importer.import("BoxTextured.gltf");
					registry->emplace<Render>(selectedEntity, std::make_shared<Model>(model));
					registry->emplace<Transform>(selectedEntity);
				}
			}
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
			sprintf(str, "%f %f %f %f", node->transform.normalTransform[0][0], node->transform.normalTransform[0][1], node->transform.normalTransform[0][2], node->transform.normalTransform[0][3]);
			ImGui::Text(str);
			sprintf(str, "%f %f %f %f", node->transform.normalTransform[1][0], node->transform.normalTransform[1][1], node->transform.normalTransform[1][2], node->transform.normalTransform[1][3]);
			ImGui::Text(str);
			sprintf(str, "%f %f %f %f", node->transform.normalTransform[2][0], node->transform.normalTransform[2][1], node->transform.normalTransform[2][2], node->transform.normalTransform[2][3]);
			ImGui::Text(str);
			sprintf(str, "%f %f %f %f", node->transform.normalTransform[3][0], node->transform.normalTransform[3][1], node->transform.normalTransform[3][2], node->transform.normalTransform[3][3]);
			ImGui::Text(str);

			for (auto& it : node->childs) {
				drawNodeHierarchy(it, nodes);
			}

			ImGui::TreePop();
		}
	}
	
	void EditTransform(Transform& transform)
	{
		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
		if (ImGui::IsKeyPressed(90))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(69))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(82)) // r Key
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];

		ImGui::InputFloat3("Tr", (float*)&transform.translation, 3);
		ImGui::InputFloat3("Rt", (float*)&transform.rotation, 3);
		ImGui::InputFloat3("Sc", (float*)&transform.scale, 3);

		if (mCurrentGizmoOperation != ImGuizmo::SCALE) {
			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
				mCurrentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
				mCurrentGizmoMode = ImGuizmo::WORLD;
		}
		static bool useSnap(false);
		if (ImGui::IsKeyPressed(83))
			useSnap = !useSnap;
		ImGui::Checkbox("", &useSnap);
		ImGui::SameLine();

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

		glm::mat4 view = camera->getViewMatrix();
		glm::mat4 perspective = camera->getPerspectiveMatrix();

		ImGuizmo::Manipulate((float*)&view, (float*)&perspective, mCurrentGizmoOperation, mCurrentGizmoMode, (float*)&transform.matrix, NULL, NULL);
	}
};