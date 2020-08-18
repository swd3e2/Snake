#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <entt/entt.hpp>
#include "../Model/Model.h"
#include "Graphics/Camera.h"
#include "Graphics/Platform/DirectX/DX11RenderTarget.h"
#include "Systems/RenderSystem.h"
#include "Import/ModelLoader.h"

class ImGuiInterface {
private:
	entt::registry* registry;
	entt::entity selectedEntity;
	bool isEntitySelected = false;
	Camera* camera;
	RenderSystem* renderSystem;
	PhysicsSystem* physicsSystem;
	DX11Window* window;
public:
	ImGuiInterface(Window* window, entt::registry* registry, Camera* camera, RenderSystem* renderSystem, PhysicsSystem* physicsSystem) :
		registry(registry), camera(camera), renderSystem(renderSystem), physicsSystem(physicsSystem)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui_ImplWin32_Init(static_cast<DX11Window*>(window)->getHwnd());
		DX11RenderContext* context = static_cast<DX11RenderContext*>(Renderer::instance()->getContext());
		ImGui_ImplDX11_Init(context->getDevice(), context->getDeviceContext());

		this->window = static_cast<DX11Window*>(window);
	}

	~ImGuiInterface() {
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	
	void update(double dt) {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Entities");
		std::string temp = "Entity №";
		int i = 0;
		if (ImGui::Button("Create entity")) {
			isEntitySelected = true;
			selectedEntity = registry->create();
		}
		if (ImGui::Button("Save")) {

		}

		registry->each([&](const entt::entity& entity) {
			if (ImGui::Selectable(("Entity###" + std::to_string(i)).c_str())) {
				selectedEntity = entity;
				isEntitySelected = true;
			}
			i++;
		});
		ImGui::End();

		ImGui::Begin("Entity Properties");
		ImGui::Text("Nodes");

		if (isEntitySelected) {
			if (registry->has<RenderComponent>(selectedEntity)) {
				RenderComponent& render = registry->get<RenderComponent>(selectedEntity);
				if (render.model != nullptr) {
					drawNodeHierarchy(render.model->rootNode, render.model->nodes);
				}
			} else {
				if (ImGui::Button("Create render component")) {
					OPENFILENAMEA ofn;         // common dialog box structure
					CHAR szFile[260] = { 0 };  // if using TCHAR macros

					// Initialize OPENFILENAME
					ZeroMemory(&ofn, sizeof(OPENFILENAME));
					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.hwndOwner = window->getHwnd();
					ofn.lpstrFile = szFile;
					ofn.nMaxFile = sizeof(szFile);
					ofn.lpstrFilter = "All\0*.*\0";
					ofn.nFilterIndex = 1;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = NULL;
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

					if (GetOpenFileNameA(&ofn) == TRUE)
					{
						ofn.lpstrFile;
						std::shared_ptr<Model> model = ModelLoader::instance()->loadFromFile(ofn.lpstrFile);
						if (model) {
							registry->emplace<RenderComponent>(selectedEntity, model);
						}
					}
				}
			}

			if (registry->has<TransformComponent>(selectedEntity)) {
				TransformComponent& transform = registry->get<TransformComponent>(selectedEntity);

				/*bool moved = false;
				moved = moved || ImGui::DragFloat3("Translation###TD", (float*)&transform.translation, 0.01f, -10000.0f, 10000.0f);
				moved = moved || ImGui::DragFloat3("Rotation###RD", (float*)&transform.rotation, 0.01f, -10000.0f, 10000.0f);
				moved = moved || ImGui::DragFloat3("Scale###SD", (float*)&transform.scale, 0.01f, -10000.0f, 10000.0f);*/

				/*if (registry->has<PhysicsComponent>(selectedEntity) && moved) {
					PhysicsComponent& physics = registry->get<PhysicsComponent>(selectedEntity);

					btTransform physicsTransform;
					physicsTransform.setIdentity();
					btQuaternion rotationQ;
					rotationQ.setEulerZYX(transform.rotation.z, transform.rotation.y, transform.rotation.x);

					physicsTransform.setOrigin(btVector3(transform.translation.x, transform.translation.y, transform.translation.z));
					physicsTransform.setRotation(rotationQ);

					physics.body->setWorldTransform(physicsTransform);
					physics.body->setCenterOfMassTransform(physicsTransform);
					physics.body->activate(true);
				}*/
			} else {
				if (ImGui::Button("Create transform component")) {
					registry->emplace<TransformComponent>(selectedEntity);
				}
			}
		} 
		
		ImGui::End();

		ImGui::Begin("Debug");
		double x = InputManager::instance()->mousePosX;
		double y = InputManager::instance()->mousePosY;
		//std::cout << "x " << x << " y " << y << std::endl;
		char str[80];
		{
			glm::mat4x4 viewproj = renderSystem->camera.getPerspectiveMatrix();
			ImGui::Text("Projection matrix");
			sprintf(str, "%f %f %f %f", viewproj[0][0], viewproj[0][1], viewproj[0][2], viewproj[0][3]);
			ImGui::Text(str);
			sprintf(str, "%f %f %f %f", viewproj[1][0], viewproj[1][1], viewproj[1][2], viewproj[1][3]);
			ImGui::Text(str);
			sprintf(str, "%f %f %f %f", viewproj[2][0], viewproj[2][1], viewproj[2][2], viewproj[2][3]);
			ImGui::Text(str);
			sprintf(str, "%f %f %f %f", viewproj[3][0], viewproj[3][1], viewproj[3][2], viewproj[3][3]);
			ImGui::Text(str);
		}
		{
			glm::mat4x4 viewproj = renderSystem->camera.getViewMatrix();

			ImGui::Text("View matrix");
			sprintf(str, "%f %f %f %f", viewproj[0][0], viewproj[0][1], viewproj[0][2], viewproj[0][3]);
			ImGui::Text(str);
			sprintf(str, "%f %f %f %f", viewproj[1][0], viewproj[1][1], viewproj[1][2], viewproj[1][3]);
			ImGui::Text(str);
			sprintf(str, "%f %f %f %f", viewproj[2][0], viewproj[2][1], viewproj[2][2], viewproj[2][3]);
			ImGui::Text(str);
			sprintf(str, "%f %f %f %f", viewproj[3][0], viewproj[3][1], viewproj[3][2], viewproj[3][3]);
			ImGui::Text(str);
		}
		{
			glm::mat4x4 viewproj = renderSystem->camera.getPerspectiveMatrix() * renderSystem->camera.getViewMatrix();
			//viewproj = glm::inverse(viewproj);

			ImGui::Text("View proj matrix");
			sprintf(str, "%f %f %f %f", viewproj[0][0], viewproj[0][1], viewproj[0][2], viewproj[0][3]);
			ImGui::Text(str);
			sprintf(str, "%f %f %f %f", viewproj[1][0], viewproj[1][1], viewproj[1][2], viewproj[1][3]);
			ImGui::Text(str);
			sprintf(str, "%f %f %f %f", viewproj[2][0], viewproj[2][1], viewproj[2][2], viewproj[2][3]);
			ImGui::Text(str);
			sprintf(str, "%f %f %f %f", viewproj[3][0], viewproj[3][1], viewproj[3][2], viewproj[3][3]);
			ImGui::Text(str);
		}
		

		ImGui::Checkbox("Enable physics", &physicsSystem->enabled);
		ImGui::DragFloat("Camera pitch", &renderSystem->camera.Pitch);
		ImGui::DragFloat("Camera yaw", &renderSystem->camera.Yaw);
		ImGui::DragFloat3("Camera position", (float*)&renderSystem->camera.Position);
		ImGui::Text(("Delta " + std::to_string(dt)).c_str());
		//ImGui::Image((void*)std::static_pointer_cast<DX11Texture2D>(renderSystem->textures["noise"])->m_TextureShaderResource, ImVec2(256, 256));
		//ImGui::Image((void*)std::static_pointer_cast<DX11Texture2D>(renderSystem->textures["ambient_occlusion_color"])->m_TextureShaderResource, ImVec2(256, 144));
		//ImGui::Image((void*)std::static_pointer_cast<DX11Texture2D>(renderSystem->textures["view_space_pos"])->m_TextureShaderResource, ImVec2(256, 144));
		//ImGui::Image((void*)std::static_pointer_cast<DX11Texture2D>(renderSystem->textures["view_space_normal"])->m_TextureShaderResource, ImVec2(256, 144));
		//ImGui::Image((void*)std::static_pointer_cast<DX11Texture2D>(renderSystem->textures["picker"])->m_TextureShaderResource, ImVec2(256, 144));
		//ImGui::Image((void*)std::static_pointer_cast<DX11Texture2D>(renderSystem->textures["bluredShadowDepthTexture"])->m_TextureShaderResource, ImVec2(256, 144));
		//ImGui::Image((void*)std::static_pointer_cast<DX11Texture2D>(renderSystem->textures["shadowColorTexture"])->m_TextureShaderResource, ImVec2(256, 144));
		//ImGui::Image((void*)std::static_pointer_cast<DX11Texture2D>(renderSystem->textures["rtt"])->m_TextureShaderResource, ImVec2(256, 144));
		//ImGui::Image((void*)std::static_pointer_cast<DX11Texture2D>(renderSystem->textures["shadowDepthTexture"])->m_TextureShaderResource, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void drawNodeHierarchy(int nodeId, const std::vector<std::shared_ptr<Node>>& nodes)
	{
		const std::shared_ptr<Node>& node = nodes[nodeId];
		if (ImGui::TreeNode(node->name.c_str())) {
			ImGui::DragFloat3(("Translation###NT" + node->name).c_str(), (float*)&node->transform.translation, 0.01f, -10000.0f, 10000.0f);
			ImGui::DragFloat3(("Scale###NS" + node->name).c_str(), (float*)&node->transform.scale, 0.01f, -10000.0f, 10000.0f);
			ImGui::DragFloat4(("Rotation###NR" + node->name).c_str(), (float*)&node->transform.rotation, 0.01f, -10000.0f, 10000.0f);

			for (auto& it : node->childs) {
				drawNodeHierarchy(it, nodes);
			}

			ImGui::TreePop();
		}
	}
};