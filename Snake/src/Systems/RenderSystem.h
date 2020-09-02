#pragma once

#include "ISystem.h"
#include "Input/InputManager.h"

#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include <entt/entt.hpp>
#include "Components.h"
#include "Graphics/vertex.h"
#include "Graphics/Camera.h"
#include "Model/Model.h"
#include <queue>
#include "FileSystem/File.h"
#include "Graphics/RenderGraph/RenderGraph.h"
#include "Import/Texture/RawTexture.h"

#include "Systems/PhysicsSystem.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <random>
#include "CommonRenderTypes.h"
#include "Graphics/Platform/DirectX/DX11Texture2D.h"
#include "Graphics/Platform/DirectX/DX11Renderer.h"
#include "EventSystem/EventSystem.h"
#include <glm/gtc/type_ptr.hpp>
#include "DebugCameraController.h"
#include "ApplicationSettings.h"
#include "Graphics/RenderQueue/RenderQueue.h"
#include <GridSystem.h>

#include "RenderSystem/RenderSystemResourceManager.h"
#include "RenderSystem/RenderGraphBuilder.h"

class RenderSystem : public ISystem
{
public:
	ApplicationSettings* settings;

    Renderer* renderer;
    RenderContext* context;
	RenderSystemResourceManager resourceManager;
	RenderSystemResource* resources;
	RenderQueue* renderQueue;
	DebugCameraController cameraController;
	RenderGraph* renderGraph;

	GridSystem gridSystem;

    PhysicsSystem* physicsSystem;

	WVP projectionData;

	Camera camera;
	Camera lightCamera;

	EventSystem* eventSystem;

	D3D11_MAPPED_SUBRESOURCE* mappedResource;

	Model* model;
	bool useDebugCamera = true;
public:
	RenderSystem(SceneManager* sceneManager, ApplicationSettings* settings, PhysicsSystem* physics, Renderer* renderer):
		ISystem(sceneManager), physicsSystem(physics), renderer(renderer), context(renderer->getContext()),
		eventSystem(eventSystem)
	{
		renderQueue = new RenderQueue();
		// Debug camera initialization
		camera.setProjectionMatrix(45.0f, (float)settings->getWindowWidth() / (float)settings->getWindowHeight(), 0.1f, 200.f);
		camera.Position = glm::vec3(8.0f, 12.0f, 0.0f);
		camera.Pitch = 55.0f;
		camera.Yaw = 3.0f;

		resources = resourceManager.initialize(renderer, settings);
		resources->point_repeat_sampler->bind(context);

		RenderGraphBuilder renderGraphBuilder;
		renderGraph = renderGraphBuilder.build(renderQueue, renderer, resources);

		auto eventSystem = EventSystem::instance();
		eventSystem->addEventListener<RenderSystem, LeftMouseClickEvent>(new ClassEventDelegate(this, &RenderSystem::onLeftMouseClick));
	}

	virtual void update(double dt) override
	{
		entt::registry* registry = sceneManager->getCurrentScene()->getRegistry();

		glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        lightProjection = lightCamera.getPerspectiveMatrix();
        lightView = glm::lookAt(lightCamera.Position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = glm::transpose(lightProjection * lightView);

        if (!useDebugCamera) {
			registry->view<CameraComponent, PlayerComponent>().each([&](CameraComponent& camera, PlayerComponent& player) {
				projectionData.projection = glm::transpose(camera.projectionMatrix * camera.viewMatrix);
			});
        } else {
			projectionData.projection = glm::transpose(camera.getPerspectiveMatrix() * camera.getViewMatrix());
        }
        
		projectionData.shadowProjection = lightSpaceMatrix;
        projectionData.shadowCameraPos = glm::vec4(lightCamera.Position, 1.0);
        projectionData.viewPos = glm::vec4(camera.Position, 1.0);
        projectionData.projectionMatrix = camera.getPerspectiveMatrix();
        projectionData.viewMatrix = glm::transpose(camera.getViewMatrix());
        projectionData.invViewMatrix = glm::inverse(projectionData.viewMatrix);
		
		renderQueue->bindShaderInputLayout(resources->shader_input_layout.get());
		renderQueue->updateConstantBuffer(resources->shader_buffer.get(), &projectionData, sizeof(WVP));

        renderGraph->execute(sceneManager->getCurrentScene());
		renderQueue->present();

		for (int i = 0; i < renderQueue->commands_size; i++) {
			auto& command = renderQueue->commands[i];
			command->execute(renderer, command);
		}
		renderQueue->clear();

		updateCamera(dt);
	}
private:
    void updateCamera(double dt) 
	{
		cameraController.update(&camera, dt);
		camera.update();
    }

	void onLeftMouseClick(Event* event) 
	{
		/*DX11Renderer* dxRenderer = (DX11Renderer*)renderer;
		ID3D11Device* device = ((DX11RenderContext*)renderer->getContext())->getDevice();
		ID3D11DeviceContext* deviceContext = ((DX11RenderContext*)renderer->getContext())->getDeviceContext();

		const unsigned char* data = nullptr;
		static ID3D11Texture2D* texture = nullptr;
		if (texture == nullptr) {
			D3D11_TEXTURE2D_DESC textureDesc = { };
			textureDesc.Height = 1;
			textureDesc.Width = 1;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
			textureDesc.Usage = D3D11_USAGE_STAGING;
			textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			textureDesc.BindFlags = 0;

			device->CreateTexture2D(&textureDesc, nullptr, &texture);
		}

		D3D11_BOX box;
		box.left = InputManager::instance()->mousePosX;
		box.right = InputManager::instance()->mousePosX + 1;
		box.top = InputManager::instance()->mousePosY;
		box.bottom = InputManager::instance()->mousePosY + 1;
		box.front = 0;
		box.back = 1;

		deviceContext->CopySubresourceRegion(texture, 0, 0, 0, 0, ((DX11Texture2D*)textures["picker"].get())->m_Texture, 0, &box);

		mappedResource = new D3D11_MAPPED_SUBRESOURCE();
		unsigned int subresource = D3D11CalcSubresource(0, 0, 0);
		deviceContext->Map(texture, subresource, D3D11_MAP_READ, 0, mappedResource);
		data = (const unsigned char*)mappedResource->pData;
		deviceContext->Unmap(texture, subresource);
		if (data != nullptr) {
			int pickedID = data[0] + data[1] * 256 + data[2] * 256 * 256;
			std::cout << pickedID << std::endl;

			registry->view<RenderComponent>().each([](RenderComponent& render) {
				render.selected = false;
				});

			if (registry->has<RenderComponent>((entt::entity)pickedID)) {
				RenderComponent& renderComponent = registry->get<RenderComponent>((entt::entity)pickedID);
				renderComponent.selected = true;
			}
		}*/

		/*double x = InputManager::instance()->mousePosX;
		double y = InputManager::instance()->mousePosY;
		glm::mat4x4 viewproj = camera.getPerspectiveMatrix() * camera.getViewMatrix();
		viewproj = glm::transpose(viewproj);
		viewproj = glm::inverse(viewproj);
		glm::vec4 temp = glm::vec4(x / 1920.0 * 2.0 - 1.0, -(y / 1080 * 2.0 - 1.0), 1.0f, 1.0f);
		temp = temp * viewproj;
		temp /= temp.w;
		std::cout << "x " << temp.x << " y " << temp.y << std::endl;

		btVector3 from = btVector3(camera.Position.x, camera.Position.y, camera.Position.z);
		btVector3 to = btVector3(temp.x, temp.y, temp.z);
		btCollisionWorld::ClosestRayResultCallback res(from, to);
		physicsSystem->dynamicsWorld->rayTest(from, to, res);

		if (res.hasHit()) {
			gridSystem.selectNode(glm::vec3(res.m_hitPointWorld.x(), 0.0f, res.m_hitPointWorld.z()));
			gridSystem.findPath(glm::vec3(0,0,0), glm::vec3(res.m_hitPointWorld.x(), 0.0f, res.m_hitPointWorld.z()));
		}*/
	}
};