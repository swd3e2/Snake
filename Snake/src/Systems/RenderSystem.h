#pragma once

#include "Input/InputManager.h"
#include "Graphics/Renderer/ShaderPipeline.h"
#include "Graphics/Renderer/ShaderInputLayout.h"
#include "Graphics/Renderer/VertexBuffer.h"
#include "Graphics/Renderer/ConstantBuffer.h"
#include "Graphics/Renderer/Texture2D.h"
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
#include "Graphics/Renderer/RenderTarget.h"
#include "Import/Texture/RawTexture.h"
#include "Graphics/RenderGraph/FullscreenPass.h"
#include "Graphics/RenderGraph/DefaultRenderPass.h"
#include "Graphics/RenderGraph/PhysicDebugRenderPass.h"
#include "Graphics/Renderer/Sampler.h"
#include "Physics/PhysicsSystem.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include "CommonRenderTypes.h"
#include "Graphics/Platform/DirectX/DX11Texture2D.h"
#include "Graphics/Platform/DirectX/DX11Renderer.h"

struct ModelData {
	glm::mat4 toWorld;
	glm::mat4 inverseToWorld;
	int selected = 0;
};

struct MeshData {
	glm::mat4 transform;
	glm::mat4 normalTransform;

	struct {
		glm::vec3 ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
		float shininess = 64.0f;
		glm::vec3 diffuseColor = glm::vec3(1.0f);
		float dummy;
		glm::vec3 specularColor = glm::vec3(1.0f);
		int hasDiffuseMap = 0;
		int hasNormalMap = 0;
	} materialData;
};

class RenderSystem {
public:
    Renderer* renderer;
    RenderContext* context;
    std::unique_ptr<RenderGraph> renderGraph;

	std::shared_ptr<FullscreenPass> lightGBufferPass;
	std::shared_ptr<Pass> guiPass;
	std::shared_ptr<FullscreenPass> testRenderPass;
	std::shared_ptr<FullscreenPass> blurPass;
	std::shared_ptr<DefaultRenderPass> gbufferPass;
	std::shared_ptr<DefaultRenderPass> pickerPass;
	std::shared_ptr<PhysicDebugRenderPass> debugPhysicsPass;
	std::shared_ptr<Pass> shadowPass;

    std::shared_ptr<ShaderInputLayout> shaderInputLayout;

    std::shared_ptr<ConstantBuffer> shaderBuffer;
	std::shared_ptr<ConstantBuffer> modelShaderBuffer;
	std::shared_ptr<ConstantBuffer> meshShaderBuffer;
	std::shared_ptr<ConstantBuffer> pickerBuffer;

    std::shared_ptr<MainRenderTarget> mainRenderTarget;
    entt::registry* registry;
  
    std::unordered_map<std::string, std::shared_ptr<Texture2D>> textures;
    std::unordered_map<std::string, std::shared_ptr<RenderTarget>> renderTargets;
    std::unordered_map<std::string, std::shared_ptr<ShaderPipeline>> shaders;

	std::shared_ptr<Sampler> pointSampler;

    PhysicsSystem* physicsSystem;
    bool useDebugCamera = true;
	int mainCamera = 0;

	WVP projectionData;
	Camera camera;
	Camera lightCamera;
	D3D11_MAPPED_SUBRESOURCE* mappedResource;
public:
	RenderSystem(entt::registry* registry, PhysicsSystem* physics, Renderer* renderer):
		physicsSystem(physics), registry(registry), renderer(renderer), context(renderer->getContext()), mainRenderTarget(std::shared_ptr<MainRenderTarget>(renderer->getMainRenderTarget()))
	{
		camera.setProjectionMatrix(90.0f, 1920.0f / 1080.0f, 0.1f, 200.f);

		shaderBuffer.reset(ConstantBuffer::create(0, sizeof(WVP), nullptr));
		modelShaderBuffer.reset(ConstantBuffer::create(1, sizeof(ModelData), nullptr));
		meshShaderBuffer.reset(ConstantBuffer::create(2, sizeof(MeshData), nullptr));
		pickerBuffer.reset(ConstantBuffer::create(3, sizeof(glm::vec4), nullptr));

		// Input layout
		{
			ShaderInputLayoutDesc desc;
			desc.elements.push_back({ InputDataType::Float3, "POSITION" });
			desc.elements.push_back({ InputDataType::Float2, "TEXCOORD" });
			desc.elements.push_back({ InputDataType::Float3, "NORMAL" });
			desc.elements.push_back({ InputDataType::Float3, "TANGENT" });
			desc.elements.push_back({ InputDataType::Float3, "BITANGENT" });
			desc.elements.push_back({ InputDataType::Int4,   "BONE" });
			desc.elements.push_back({ InputDataType::Float4, "WEIGHT" });

			shaderInputLayout.reset(ShaderInputLayout::create(desc));
		}
		// Samplers
		{
			SamplerDesc desc;
			desc.minFilterModel = FilterMode::POINT;
			desc.magFilterMode = FilterMode::POINT;
			desc.mipFilterMode = FilterMode::NONE;
			desc.addressingMode = AddressingMode::WRAP;
			pointSampler.reset(Sampler::create(desc));
			pointSampler->setSamplerUnit(0);
			pointSampler->bind(context);
		}
		// Textures
		{
 			textures["positions"] = std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 0, nullptr, TextureFormat::RGBA32F, TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
			textures["normals"] = std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 1, nullptr, TextureFormat::RGBA32F, TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
			textures["diffuse"] = std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 2, nullptr, TextureFormat::RGBA32F, TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
			textures["rtt"] = std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 0, nullptr, TextureFormat::RGBA32F, TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
			textures["shadowColorTexture"] = std::shared_ptr<Texture2D>(Texture2D::create(1024, 1024, 5, nullptr, TextureFormat::RGBA32F, TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
			textures["bluredShadowDepthTexture"] = std::shared_ptr<Texture2D>(Texture2D::create(1024, 1024, 5, nullptr, TextureFormat::RG32F, TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
			textures["shadowDepthTexture"] = std::shared_ptr<Texture2D>(Texture2D::create(1024, 1024, 5, nullptr, TextureFormat::D24S8, TextureFlags::TF_DepthBuffer | TextureFlags::TF_ShaderResource));
			textures["renderTargetDepthTexture"] = std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 3, nullptr, TextureFormat::D32, TextureFlags::TF_DepthBuffer | TextureFlags::TF_ShaderResource));
			textures["picker"] = std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 0, nullptr, TextureFormat::RGBA8, TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
			textures["pickerDepth"] = std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 0, nullptr, TextureFormat::D24S8, TextureFlags::TF_DepthBuffer | TextureFlags::TF_ShaderResource));
		}
		// Render targets
		{
			{
				std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
				renderTarget->setColorTexture(textures["positions"], 0);
				renderTarget->setColorTexture(textures["normals"], 1);
				renderTarget->setColorTexture(textures["diffuse"], 2);
				renderTarget->setDepthTexture(textures["renderTargetDepthTexture"]);
				renderTargets["gBuffer"] = renderTarget;
			}
			{
				std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
				renderTarget->setColorTexture(textures["shadowColorTexture"], 0);
				renderTarget->setDepthTexture(textures["shadowDepthTexture"]);
				renderTargets["shadow"] = renderTarget;
			}
			{
				std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
				renderTarget->setColorTexture(textures["rtt"], 0);
				renderTargets["light"] = renderTarget;
			}
			{
				std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
				renderTarget->setColorTexture(textures["bluredShadowDepthTexture"], 0);
				renderTargets["blur"] = renderTarget;
			}
			// Selection render target
			{
				std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
				renderTarget->setColorTexture(textures["picker"], 0);
				renderTarget->setDepthTexture(textures["pickerDepth"], 0);
				renderTargets["picker"] = renderTarget;
			}
		}
		// Shaders
		{
			createShaderFromFolder("shaders/dx/default", "default");
			createShaderFromFolder("shaders/dx/gbuffer", "gbuffer");
			createShaderFromFolder("shaders/dx/shadow", "shadow");
			createShaderFromFolder("shaders/dx/light", "light");
			createShaderFromFolder("shaders/dx/blur", "blur");
			createShaderFromFolder("shaders/dx/physdebug", "physdebug");
			createShaderFromFolder("shaders/dx/gui", "gui");
			createShaderFromFolder("shaders/dx/picker", "picker");
		}
		// Render passess
		renderGraph = std::make_unique<RenderGraph>(renderer);
		{
			gbufferPass.reset(new DefaultRenderPass("GBufferPass", registry));
			gbufferPass->setRenderTarget(renderTargets["gBuffer"]);
			gbufferPass->setShader(shaders["gbuffer"]);
			gbufferPass->func = [registry = registry, modelShaderBuffer= modelShaderBuffer, meshShaderBuffer= meshShaderBuffer](Renderer* renderer, entt::entity entity) {
				ModelData modelData;
				MeshData meshData;

				TransformComponent& transform = registry->get<TransformComponent>(entity);
				RenderComponent& render = registry->get<RenderComponent>(entity);

				modelData.toWorld = glm::transpose(transform.matrix);
				modelData.inverseToWorld = glm::inverse(modelData.toWorld);
				modelData.selected = render.selected;

				modelShaderBuffer->update((void*)&modelData);
				modelShaderBuffer->bind(renderer->getContext());

				std::unordered_map<int, std::vector<std::shared_ptr<SubMesh>>>& submeshes = render.model->submeshes;
				const std::vector<std::shared_ptr<Material>>& materials = render.model->materials;

				for (auto& node : render.model->nodes) {
					if (node->mesh == -1) continue;

					const std::vector<std::shared_ptr<SubMesh>>& submeshs = submeshes[node->mesh];

					for (auto& submesh : submeshs) {
						if (materials.size() > submesh->material) {
							const std::shared_ptr<Material>& material = materials[submesh->material];

							if (material->diffuseTexture) {
								material->diffuseTexture->bindToUnit(0, renderer->getContext());
								meshData.materialData.hasDiffuseMap = true;
							}
							else {
								meshData.materialData.hasDiffuseMap = false;
							}

							if (material->normalTexture) {
								material->normalTexture->bindToUnit(1, renderer->getContext());
								meshData.materialData.hasNormalMap = true;
							}
							else {
								meshData.materialData.hasNormalMap = false;
							}
						} else {
							meshData.materialData.diffuseColor = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
							meshData.materialData.hasDiffuseMap = false;
							meshData.materialData.hasNormalMap = false;
						}

						meshData.transform = glm::transpose(node->transform.worldTransform);
						meshData.normalTransform = glm::transpose(node->transform.normalTransform);

						meshShaderBuffer->update(&meshData);
						meshShaderBuffer->bind(renderer->getContext());

						submesh->vBuffer->bind(renderer->getContext());
						submesh->iBuffer->bind(renderer->getContext());

						renderer->drawIndexed(submesh->iBuffer->getSize());
					}
				};
			};

			renderGraph->addPass(gbufferPass);
		}
		{
			debugPhysicsPass.reset(new PhysicDebugRenderPass(physics, "DebugPhysicsPass"));
			debugPhysicsPass->setRenderTarget(renderTargets["gBuffer"]);
			debugPhysicsPass->setShader(shaders["physdebug"]);

			renderGraph->addPass(debugPhysicsPass);
		}
		{
			pickerPass.reset(new DefaultRenderPass("Picker pass", registry));
			pickerPass->setRenderTarget(renderTargets["picker"]);
			pickerPass->setShader(shaders["picker"]);
			pickerPass->func = [registry = registry, modelShaderBuffer = modelShaderBuffer, meshShaderBuffer = meshShaderBuffer, pickerBuffer = pickerBuffer](Renderer* renderer, entt::entity entity) {
				ModelData modelData;
				MeshData meshData;

				TransformComponent& transform = registry->get<TransformComponent>(entity);
				RenderComponent& render = registry->get<RenderComponent>(entity);

				modelData.toWorld = glm::transpose(transform.matrix);
				modelData.inverseToWorld = glm::inverse(modelData.toWorld);
				modelShaderBuffer->update((void*)&modelData);
				modelShaderBuffer->bind(renderer->getContext());

				glm::vec4 color;
				color.r = (((int)entity & (0x000000FF)) >> 0) / 255.0f;
				color.g = (((int)entity & (0x0000FF00)) >> 8) / 255.0f;
				color.b = (((int)entity & (0x00FF0000)) >> 16) / 255.0f;
				color.a = 1.0f;

				pickerBuffer->update((void*)&color);
				pickerBuffer->bind(renderer->getContext());

				std::unordered_map<int, std::vector<std::shared_ptr<SubMesh>>>& submeshes = render.model->submeshes;
				const std::vector<std::shared_ptr<Material>>& materials = render.model->materials;

				for (auto& node : render.model->nodes) {
					if (node->mesh == -1) continue;

					const std::vector<std::shared_ptr<SubMesh>>& submeshs = submeshes[node->mesh];

					for (auto& submesh : submeshs) {
						meshData.transform = glm::transpose(node->transform.worldTransform);
						meshData.normalTransform = glm::transpose(node->transform.normalTransform);

						meshShaderBuffer->update(&meshData);
						meshShaderBuffer->bind(renderer->getContext());

						submesh->vBuffer->bind(renderer->getContext());
						submesh->iBuffer->bind(renderer->getContext());

						renderer->drawIndexed(submesh->iBuffer->getSize());
					}
				};
			};
			renderGraph->addPass(pickerPass);
		}
		{
			shadowPass.reset(new Pass("ShadowPass"));
			shadowPass->setRenderTarget(renderTargets["shadow"]); 
			shadowPass->setShader(shaders["shadow"]);
			renderGraph->addPass(shadowPass);
		}
		{
			blurPass.reset(new FullscreenPass("GaussianBlurPass"));
			blurPass->setRenderTarget(renderTargets["blur"]);
			blurPass->setShader(shaders["blur"]);
			blurPass->setTexture(0, textures["shadowColorTexture"]);
			renderGraph->addPass(blurPass);
		}
		{
			lightGBufferPass.reset(new FullscreenPass("LightPass"));
			lightGBufferPass->setRenderTarget(renderTargets["light"]);

			lightGBufferPass->setShader(shaders["light"]);
			lightGBufferPass->setTexture(0, textures["positions"]);
			lightGBufferPass->setTexture(1, textures["normals"]);
			lightGBufferPass->setTexture(2, textures["diffuse"]);
			lightGBufferPass->setTexture(3, textures["bluredShadowDepthTexture"]);
			lightGBufferPass->setTexture(4, textures["renderTargetDepthTexture"]);
			renderGraph->addPass(lightGBufferPass);
		}
		{
			guiPass.reset(new Pass("GuiPass"));
			guiPass->setRenderTarget(renderTargets["light"]);
			guiPass->setShader(shaders["gui"]);

			renderGraph->addPass(guiPass);
		}
		{
			testRenderPass.reset(new FullscreenPass("MainPass"));
			testRenderPass->setRenderTarget(mainRenderTarget);
			testRenderPass->setShader(shaders["default"]);
			testRenderPass->setTexture(2, textures["rtt"]);
			renderGraph->addPass(testRenderPass);
		}
	}

	void update(double dt) {
        for (auto& it : renderTargets) {
            it.second->clear(context);
        }

		mainRenderTarget->clear(context);
		shaderInputLayout->bind(context);

		glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        lightProjection = lightCamera.getPerspectiveMatrix();
        lightView = glm::lookAt(glm::vec3(lightCamera.m_Position), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = glm::transpose(lightProjection * lightView);

        if (!useDebugCamera) {
			registry->view<CameraComponent, PlayerComponent>().each([&](CameraComponent& camera, PlayerComponent& player) {
				projectionData.projection = glm::transpose(camera.projectionMatrix * camera.viewMatrix);
			});
        } else {
			projectionData.projection = glm::transpose(camera.getPerspectiveMatrix() * camera.getViewMatrix());
        }
        
		projectionData.shadowProjection = lightSpaceMatrix;
        projectionData.shadowCameraPos = lightCamera.m_Position;
        projectionData.viewPos = camera.m_Position;
		
		shaderBuffer->update((void*)&projectionData);
		shaderBuffer->bind(context);

        registry->view<TransformComponent, RenderComponent>().each([&](entt::entity entity, TransformComponent& transform, RenderComponent& render) {
			updateTransform(render.model->rootNode, render.model.get(), glm::mat4(1.0f));
			gbufferPass->addEntity(entity);
			pickerPass->addEntity(entity);
        });

        renderGraph->execute();

		updateCamera(dt);

		if (InputManager::instance()->isMouseKeyPressed(1)) {
			DX11Renderer* dxRenderer = (DX11Renderer*)renderer;
			ID3D11Device* device = ((DX11RenderContext*)renderer->getContext())->getDevice();
			ID3D11DeviceContext* deviceContext = ((DX11RenderContext*)renderer->getContext())->getDeviceContext();

			const char* data = nullptr;
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
			data = (const char*)mappedResource->pData;
			deviceContext->Unmap(texture, subresource);

			int pickedID = data[0] + data[1] * 256 + data[2] * 256 * 256;
			std::cout << pickedID << std::endl;

			registry->view<RenderComponent>().each([](RenderComponent& render) {
				render.selected = false;
			});

			if (registry->has<RenderComponent>((entt::entity)pickedID)) {
				RenderComponent& renderComponent = registry->get<RenderComponent>((entt::entity)pickedID);
				renderComponent.selected = true;
			}
		}
	}

    void updateCamera(double dt) {
		if (InputManager::instance()->instance()->isKeyPressed(37)) {
			lightCamera.m_Position.x += 0.002f;
		} else if (InputManager::instance()->instance()->isKeyPressed(39)) {
			lightCamera.m_Position.x -= 0.002f;
		}

		if (InputManager::instance()->instance()->isKeyPressed(38)) {
			lightCamera.m_Position.z += 0.002f;
		} else if (InputManager::instance()->instance()->isKeyPressed(40)) {
			lightCamera.m_Position.z -= 0.002f;
		}

        if (!useDebugCamera) return;
        if (InputManager::instance()->isKeyPressed(31)) {
            mainCamera = 0;
        } else if (InputManager::instance()->isKeyPressed(32)) {
            mainCamera = 1;
        }
		
        if (InputManager::instance()->isKeyPressed(69)) {
            camera.m_Rotation.x -= (float)InputManager::instance()->mouseMoveX * 2.0f * (float)dt / 1000.0f;
            camera.m_Rotation.y -= (float)InputManager::instance()->mouseMoveY * 2.0f *  (float)dt / 1000.0f;
        }

        if (InputManager::instance()->instance()->isKeyPressed(87)) {
            camera.m_Position -= camera.forwardVec * 4.0f * (float)dt / 1000.0f;
        } else if (InputManager::instance()->instance()->isKeyPressed(83)) {
            camera.m_Position += camera.forwardVec * 4.0f * (float)dt / 1000.0f;
        }

        if (InputManager::instance()->instance()->isKeyPressed(65)) {
            camera.m_Position -= camera.rightVec * 4.0f * (float)dt / 1000.0f;
        } else if (InputManager::instance()->instance()->isKeyPressed(68)) {
            camera.m_Position += camera.rightVec * 4.0f * (float)dt / 1000.0f;
        }
		
        camera.update();
    }

    void updateTransform(const int nodeId, Model* renderable, const glm::mat4& parentTransform) {
        const std::vector<std::shared_ptr<Node>>& nodes = renderable->nodes;
        if (nodes.size() <= nodeId) return;

        const std::shared_ptr<Node>& node = nodes[nodeId];
		
        if (node->transform.hasChanged == true) {
            glm::mat4 localTransform = glm::mat4(1.0f);
            localTransform = glm::translate(localTransform, node->transform.translation);
            localTransform = localTransform * glm::toMat4(node->transform.rotation);
            localTransform = glm::scale(localTransform, node->transform.scale);

            node->transform.worldTransform = parentTransform * localTransform;
            node->transform.normalTransform = glm::inverse(node->transform.worldTransform);
            node->transform.hasChanged = false;
        }

        for (auto& it : node->childs) {
            updateTransform(it, renderable, node->transform.worldTransform);
        }
    }

    void createShaderFromFolder(const std::string& folder, const std::string& name) {
		std::string vertexShaderFilename;
		std::string pixelShaderFilename;
		std::string geometryShaderFilename;

        if (Renderer::getType() == RendererType::OpenGL) {
            vertexShaderFilename = "/pixel.glsl";
            pixelShaderFilename = "/vertex.glsl";
            geometryShaderFilename = "/geometry.glsl";
        } else {
			vertexShaderFilename = "/vertex.hlsl";
			pixelShaderFilename = "/pixel.hlsl";
			geometryShaderFilename = "/geometry.hlsl";
        }

		File filePS(folder + pixelShaderFilename);
		File fileVS(folder + vertexShaderFilename);
		File fileGS(folder + geometryShaderFilename);

        if (!filePS.exists() || !fileVS.exists()) {
            std::cout << "Could not create shader from path " << folder << std::endl;
            return;
        }

        shaders[name] = std::shared_ptr<ShaderPipeline>(ShaderPipeline::create(fileVS.getConent(), filePS.getConent()));
    }
};