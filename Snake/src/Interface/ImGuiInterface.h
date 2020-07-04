#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <entt/entt.hpp>
#include "../Model/Model.h"
#include "imgui/ImGuizmo.h"
#include "Graphics/Camera.h"
#include "Graphics/Platform/DirectX/DX11RenderTarget.h"
#include "Saver.h"
#include "Systems/RenderSystem.h"

class ImGuiInterface {
private:
	entt::registry* registry;
	entt::entity selectedEntity;
	bool isEntitySelected = false;
	Camera* camera;
	RenderSystem* renderSystem;
public:
	ImGuiInterface(Window* window, entt::registry* registry, Camera* camera, RenderSystem* renderSystem) :
		registry(registry), camera(camera), renderSystem(renderSystem)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui_ImplWin32_Init(static_cast<DX11Window*>(window)->getHwnd());
		DX11RenderContext* context = static_cast<DX11RenderContext*>(Renderer::instance()->getContext());
		ImGui_ImplDX11_Init(context->getDevice(), context->getDeviceContext());
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
		ImGui::Image((void*)std::static_pointer_cast<DX11Texture2D>(renderSystem->textures["bluredShadowDepthTexture"])->m_TextureShaderResource, ImVec2(256, 144), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::Image((void*)std::static_pointer_cast<DX11Texture2D>(renderSystem->textures["shadowColorTexture"])->m_TextureShaderResource, ImVec2(256, 144), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::Image((void*)std::static_pointer_cast<DX11Texture2D>(renderSystem->textures["rtt"])->m_TextureShaderResource, ImVec2(256, 144), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::Image((void*)std::static_pointer_cast<DX11Texture2D>(renderSystem->textures["shadowDepthTexture"])->m_TextureShaderResource, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
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
				EditTransform(transform);
			} else {
				if (ImGui::Button("Create render component")) {
					/*GltfImporter importer;
					std::shared_ptr<Import::Model> model = importer.import("BoxTextured.gltf");
					registry->emplace<Render>(selectedEntity, std::make_shared<Model>(model));
					registry->emplace<Transform>(selectedEntity);*/
				}
			}
		} 
		
		ImGui::End();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			mCurrentGizmoOperation = ImGuizmo::SCALE;

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

		glm::mat4 view = camera->getViewMatrix();
		glm::mat4 perspective = camera->getPerspectiveMatrix();

		static float* delta = new float[15];
		ImGuizmo::Manipulate((float*)&view, (float*)&perspective, mCurrentGizmoOperation, ImGuizmo::LOCAL, (float*)&transform.matrix, delta, NULL);
		float matrixTranslationDelta[3], matrixRotationDelta[3], matrixScaleDelta[3];
		ImGuizmo::DecomposeMatrixToComponents(delta, matrixTranslationDelta, matrixRotationDelta, matrixScaleDelta);
		ImGui::DragFloat3("Translation Delta###TD", (float*)&matrixTranslationDelta, 0.01f, -10000.0f, 10000.0f);
		ImGui::DragFloat3("Rotation Delta###RD", (float*)&matrixRotationDelta, 0.01f, -10000.0f, 10000.0f);
		ImGui::DragFloat3("Scale Delta###SD", (float*)&matrixScaleDelta, 0.01f, -10000.0f, 10000.0f);

		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents((float*)&transform.matrix, matrixTranslation, matrixRotation, matrixScale);

		transform.translation.x = matrixTranslation[0];
		transform.translation.y = matrixTranslation[1];
		transform.translation.z = matrixTranslation[2];

		transform.rotation.x = matrixRotation[0];
		transform.rotation.y = matrixRotation[1];
		transform.rotation.z = matrixRotation[2];

		transform.scale.x = matrixScale[0];
		transform.scale.y = matrixScale[1];
		transform.scale.z = matrixScale[2];

		ImGui::DragFloat3("Translation###T", (float*)&matrixTranslation, 0.01f, -10000.0f, 10000.0f);
		ImGui::DragFloat3("Rotation###R", (float*)&matrixRotation, 0.01f, -10000.0f, 10000.0f);
		ImGui::DragFloat3("Scale###S", (float*)&matrixScale, 0.01f, -10000.0f, 10000.0f);

		/*btTransform transform;
		transform.setIdentity();

		transform.setOrigin(btVector3(position.x, position.y, position.z));
		transform.setRotation(body->getWorldTransform().getRotation());
		body->setWorldTransform(transform);
		body->activate(true);*/

		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (float*)&transform.matrix);
	}
};