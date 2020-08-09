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
#include <random>
#include "CommonRenderTypes.h"
#include "Graphics/Platform/DirectX/DX11Texture2D.h"
#include "Graphics/Platform/DirectX/DX11Renderer.h"
#include "EventSystem/EventSystem.h"
#include <glm/gtc/type_ptr.hpp>
#include <GridSystem.h>

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

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

class RenderSystem : public EventListener<RenderSystem> {
public:
	GridSystem gridSystem;
    Renderer* renderer;
    RenderContext* context;
    std::unique_ptr<RenderGraph> renderGraph;

	std::shared_ptr<FullscreenPass> lightGBufferPass;
	std::shared_ptr<FullscreenPass> aoPass;
	std::shared_ptr<FullscreenPass> aoBlurPass;
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
	std::shared_ptr<ConstantBuffer> ambientOcclusionBuffer;

    std::shared_ptr<MainRenderTarget> mainRenderTarget;
    entt::registry* registry;
  
    std::unordered_map<std::string, std::shared_ptr<Texture2D>> textures;
    std::unordered_map<std::string, std::shared_ptr<RenderTarget>> renderTargets;
    std::unordered_map<std::string, std::shared_ptr<ShaderPipeline>> shaders;

	std::shared_ptr<Sampler> pointSampler;
	std::shared_ptr<Sampler> pointRepeatSampler;
	std::shared_ptr<Sampler> linearClampSampler;

    PhysicsSystem* physicsSystem;
    bool useDebugCamera = true;
	int mainCamera = 0;

	WVP projectionData;
	Camera camera;
	Camera lightCamera;
	EventSystem* eventSystem;
	D3D11_MAPPED_SUBRESOURCE* mappedResource;

	std::shared_ptr<VertexBuffer> quadVertexBuffer;
	std::shared_ptr<IndexBuffer> quadIndexBuffer;
public:
	RenderSystem(entt::registry* registry, PhysicsSystem* physics, Renderer* renderer, EventSystem* eventSystem):
		physicsSystem(physics), registry(registry), renderer(renderer), context(renderer->getContext()), mainRenderTarget(std::shared_ptr<MainRenderTarget>(renderer->getMainRenderTarget())),
		eventSystem(eventSystem)
	{
		eventSystem->addEventListener<RenderSystem, LeftMouseClickEvent>(this, &RenderSystem::onLeftMouseClick);
		camera.setProjectionMatrix(45.0f, 1920.0f / 1080.0f, 0.1f, 200.f);
		camera.Position = glm::vec3(8.0f, 12.0f, 0.0f);
		camera.Pitch = 55.0f;
		camera.Yaw = 3.0f;

		shaderBuffer.reset(ConstantBuffer::create(0, sizeof(WVP), nullptr));
		modelShaderBuffer.reset(ConstantBuffer::create(1, sizeof(ModelData), nullptr));
		meshShaderBuffer.reset(ConstantBuffer::create(2, sizeof(MeshData), nullptr));
		pickerBuffer.reset(ConstantBuffer::create(3, sizeof(glm::vec4), nullptr));
		ambientOcclusionBuffer.reset(ConstantBuffer::create(2, sizeof(glm::vec4) * 64, nullptr));

		{
			std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
			std::default_random_engine generator;
			std::vector<glm::vec4> ssaoKernel;
			for (unsigned int i = 0; i < 64; ++i)
			{
				glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
				sample = glm::normalize(sample);
				sample *= randomFloats(generator);
				float scale = float(i) / 64.0;

				// scale samples s.t. they're more aligned to center of kernel
				scale = lerp(0.1f, 1.0f, scale * scale);
				sample *= scale;
				ssaoKernel.push_back(glm::vec4(sample, 0));
			}
			ambientOcclusionBuffer->update(ssaoKernel.data());

			std::vector<glm::vec3> ssaoNoise;
			for (unsigned int i = 0; i < 16; i++)
			{
				glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
				ssaoNoise.push_back(noise);
			}

 			textures["noise"] = std::shared_ptr<Texture2D>(Texture2D::create(4, 4, 0, ssaoNoise.data(), TextureFormat::RGB32F, TextureFlags::TF_ShaderResource));
		}

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
			desc.mipFilterMode = FilterMode::POINT;
			desc.addressingMode = AddressingMode::WRAP;
			pointRepeatSampler.reset(Sampler::create(desc));
			pointRepeatSampler->setSamplerUnit(0);
			pointRepeatSampler->bind(context);
		}
		{
			SamplerDesc desc;
			desc.minFilterModel = FilterMode::POINT;
			desc.magFilterMode = FilterMode::POINT;
			desc.mipFilterMode = FilterMode::POINT;
			desc.addressingMode = AddressingMode::CLAMP;
			pointSampler.reset(Sampler::create(desc));
			pointSampler->setSamplerUnit(1);
			pointSampler->bind(context);
		}
		{
			SamplerDesc desc;
			desc.minFilterModel = FilterMode::LINEAR;
			desc.magFilterMode = FilterMode::LINEAR;
			desc.mipFilterMode = FilterMode::LINEAR;
			desc.addressingMode = AddressingMode::CLAMP;
			linearClampSampler.reset(Sampler::create(desc));
			linearClampSampler->setSamplerUnit(2);
			linearClampSampler->bind(context);
		}
		// Textures
		{
 			textures["positions"]					= std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 0, nullptr, TextureFormat::RGBA32F,	TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
			textures["normals"]						= std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 1, nullptr, TextureFormat::RGBA32F,	TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
			textures["diffuse"]						= std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 2, nullptr, TextureFormat::RGBA32F,	TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
			textures["view_space_pos"]				= std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 2, nullptr, TextureFormat::RGBA32F,	TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
			textures["view_space_normal"]			= std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 2, nullptr, TextureFormat::RGBA16F,	TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
			textures["rtt"]							= std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 0, nullptr, TextureFormat::RGBA16F,	TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
		
			textures["g_buffer_depth"]				= std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 3, nullptr, TextureFormat::D24S8,	TextureFlags::TF_DepthBuffer  | TextureFlags::TF_ShaderResource));
			textures["picker"]						= std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 0, nullptr, TextureFormat::RGBA8,	TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
			textures["pickerDepth"]					= std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 0, nullptr, TextureFormat::D24S8,	TextureFlags::TF_DepthBuffer  | TextureFlags::TF_ShaderResource));
			textures["ambient_occlusion_color"]		= std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 0, nullptr, TextureFormat::RGBA8,	TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));

			textures["shadowColorTexture"]			= std::shared_ptr<Texture2D>(Texture2D::create(1024, 1024, 5, nullptr, TextureFormat::RGBA32F,  TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
			textures["bluredShadowDepthTexture"]	= std::shared_ptr<Texture2D>(Texture2D::create(1024, 1024, 5, nullptr, TextureFormat::RGBA32F,	TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
			textures["shadowDepthTexture"]			= std::shared_ptr<Texture2D>(Texture2D::create(1024, 1024, 5, nullptr, TextureFormat::D24S8,	TextureFlags::TF_DepthBuffer  | TextureFlags::TF_ShaderResource));
		}
		// Render targets
		{
			{
				std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
				renderTarget->setColorTexture(textures["positions"], 0);
				renderTarget->setColorTexture(textures["normals"], 1);
				renderTarget->setColorTexture(textures["diffuse"], 2);
				renderTarget->setColorTexture(textures["view_space_pos"], 3);
				renderTarget->setColorTexture(textures["view_space_normal"], 4);
				renderTarget->setDepthTexture(textures["g_buffer_depth"]);
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
			// Ambient occlusion
			{
				std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
				renderTarget->setColorTexture(textures["ambient_occlusion_color"], 0);
				renderTargets["ambient_occlusion"] = renderTarget;
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
			createShaderFromFolder("shaders/dx/ambient_occlusion", "ambient_occlusion");
		}
		// Render passess
		renderGraph = std::make_unique<RenderGraph>(renderer);
		{
			gbufferPass.reset(new DefaultRenderPass("GBufferPass", registry));
			gbufferPass->setRenderTarget(renderTargets["gBuffer"]);
			gbufferPass->setShader(shaders["gbuffer"]);

			gbufferPass->setConstantBuffer(shaderBuffer);
			gbufferPass->setConstantBuffer(modelShaderBuffer);
			gbufferPass->setConstantBuffer(meshShaderBuffer);

			std::vector<vertex> vertexData;
			vertexData.push_back(vertex( 0.49f, -0.63f,  0.49f, 1.0f, 0.0f));
			vertexData.push_back(vertex( 0.49f, -0.63f, -0.49f, 1.0f, 1.0f));
			vertexData.push_back(vertex(-0.49f, -0.63f,  0.49f, 0.0f, 0.0f));
			vertexData.push_back(vertex(-0.49f, -0.63f, -0.49f, 0.0f, 1.0f));
			quadVertexBuffer.reset(VertexBuffer::create(vertexData.size(), sizeof(vertex), vertexData.data()));

			std::vector<unsigned int> indexData;
			indexData.push_back(0); indexData.push_back(2); indexData.push_back(1);
			indexData.push_back(2); indexData.push_back(3); indexData.push_back(1);
			quadIndexBuffer.reset(IndexBuffer::create(indexData.size(), indexData.data()));

			gbufferPass->func2 = [
				gridSystem = &gridSystem,
				quadVertexBuffer = quadVertexBuffer,
				quadIndexBuffer = quadIndexBuffer,
				modelShaderBuffer = modelShaderBuffer,
				meshShaderBuffer = meshShaderBuffer,
				renderer = renderer
			] () {
				quadVertexBuffer->bind(renderer->getContext());
				quadIndexBuffer->bind(renderer->getContext());

				MeshData meshData;
				
				meshData.materialData.hasDiffuseMap = false;
				meshData.materialData.hasNormalMap = false;
				meshData.materialData.diffuseColor = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
				meshData.transform = glm::mat4(1.0f);

				meshShaderBuffer->update(&meshData);
				meshShaderBuffer->bind(renderer->getContext());

				for (auto& it : gridSystem->nodes) {
					for (auto& node : it) {
						ModelData modelData;

						modelData.toWorld = glm::transpose(glm::scale(glm::translate(glm::mat4(1.0f), node.position), glm::vec3(gridSystem->cellSize, 1.0f, gridSystem->cellSize)));
						//modelData.inverseToWorld = glm::inverse(modelData.toWorld);

						if (!node.walkable) {
							meshData.materialData.diffuseColor = glm::vec4(1.0f, 0.2f, 0.0f, 1.0f);
						} else if (gridSystem->path.find(&node) != gridSystem->path.end()) {
							meshData.materialData.diffuseColor = glm::vec4(0.5f, 1.0f, 0.0f, 1.0f);
						} else {
							meshData.materialData.diffuseColor = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
						}

						meshShaderBuffer->update(&meshData);
						meshShaderBuffer->bind(renderer->getContext());

						modelShaderBuffer->update((void*)&modelData);
						modelShaderBuffer->bind(renderer->getContext());

						renderer->drawIndexed(6);
					}
				}
			};

			gbufferPass->func = [
				registry = registry, 
				modelShaderBuffer= modelShaderBuffer, 
				meshShaderBuffer= meshShaderBuffer
			] (Renderer* renderer, entt::entity entity) {
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
							} else {
								meshData.materialData.hasDiffuseMap = false;
							}

							if (material->normalTexture) {
								material->normalTexture->bindToUnit(1, renderer->getContext());
								meshData.materialData.hasNormalMap = true;
							} else {
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
			debugPhysicsPass.reset(new PhysicDebugRenderPass(physics, "Debug Physics Pass"));
			debugPhysicsPass->setRenderTarget(renderTargets["gBuffer"]);
			debugPhysicsPass->setShader(shaders["physdebug"]);

			debugPhysicsPass->setConstantBuffer(shaderBuffer);

			renderGraph->addPass(debugPhysicsPass);
		}
		{
			pickerPass.reset(new DefaultRenderPass("Picker pass", registry));
			pickerPass->setRenderTarget(renderTargets["picker"]);
			pickerPass->setShader(shaders["picker"]);

			pickerPass->setConstantBuffer(shaderBuffer);
			pickerPass->setConstantBuffer(modelShaderBuffer);
			pickerPass->setConstantBuffer(meshShaderBuffer);

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
		//{
		//	shadowPass.reset(new Pass("Shadow Pass"));
		//	shadowPass->setRenderTarget(renderTargets["shadow"]); 
		//	shadowPass->setShader(shaders["shadow"]);

		//	renderGraph->addPass(shadowPass);
		//}
		//{
		//	blurPass.reset(new FullscreenPass("Gaussian Blur Pass"));
		//	blurPass->setRenderTarget(renderTargets["blur"]);
		//	blurPass->setShader(shaders["blur"]);
		//	blurPass->setTexture(0, textures["shadowColorTexture"]);

		//	renderGraph->addPass(blurPass);
		//}
		{
			aoPass.reset(new FullscreenPass("Ambient Occlusion Pass"));
			aoPass->setRenderTarget(renderTargets["ambient_occlusion"]);

			aoPass->setShader(shaders["ambient_occlusion"]);
			aoPass->setTexture(0, textures["view_space_pos"]);
			aoPass->setTexture(1, textures["normals"]);
			aoPass->setTexture(2, textures["view_space_normal"]);
			aoPass->setTexture(3, textures["noise"]);
			aoPass->setTexture(4, textures["g_buffer_depth"]);
			aoPass->setConstantBuffer(ambientOcclusionBuffer);

			renderGraph->addPass(aoPass);
		}
		{
			aoBlurPass.reset(new FullscreenPass("Ambient occluseion blur pass"));
			aoBlurPass->setRenderTarget(renderTargets["blur"]);
			aoBlurPass->setShader(shaders["blur"]);
			aoBlurPass->setTexture(0, textures["ambient_occlusion_color"]);

			renderGraph->addPass(aoBlurPass);
		}
		{
			lightGBufferPass.reset(new FullscreenPass("Light Pass"));
			lightGBufferPass->setRenderTarget(renderTargets["light"]);

			lightGBufferPass->setConstantBuffer(shaderBuffer);
			lightGBufferPass->setConstantBuffer(modelShaderBuffer);
			lightGBufferPass->setConstantBuffer(meshShaderBuffer);

			lightGBufferPass->setShader(shaders["light"]);
			lightGBufferPass->setTexture(0, textures["positions"]);
			lightGBufferPass->setTexture(1, textures["normals"]);
			lightGBufferPass->setTexture(2, textures["diffuse"]);
			lightGBufferPass->setTexture(3, textures["bluredShadowDepthTexture"]);
			lightGBufferPass->setTexture(4, textures["g_buffer_depth"]);

			renderGraph->addPass(lightGBufferPass);
		}
		{
			// @todo: Not used
		/*	guiPass.reset(new Pass("GuiPass"));
			guiPass->setRenderTarget(renderTargets["light"]);
			guiPass->setShader(shaders["gui"]);

			renderGraph->addPass(guiPass);*/
		}
		{
			testRenderPass.reset(new FullscreenPass("Result Pass"));
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
		
		shaderBuffer->update((void*)&projectionData);
		shaderBuffer->bind(context);

        registry->view<TransformComponent, RenderComponent>().each([&](entt::entity entity, TransformComponent& transform, RenderComponent& render) {
			updateTransform(render.model->rootNode, render.model.get(), glm::mat4(1.0f));
			gbufferPass->addEntity(entity);
			pickerPass->addEntity(entity);
        });

        renderGraph->execute();

		updateCamera(dt);
	}

    void updateCamera(double dt) {
		if (InputManager::instance()->instance()->isKeyPressed(37)) {
			lightCamera.Position.x += 0.002f;
		} else if (InputManager::instance()->instance()->isKeyPressed(39)) {
			lightCamera.Position.x -= 0.002f;
		}

		if (InputManager::instance()->instance()->isKeyPressed(38)) {
			lightCamera.Position.z += 0.002f;
		} else if (InputManager::instance()->instance()->isKeyPressed(40)) {
			lightCamera.Position.z -= 0.002f;
		}

        if (!useDebugCamera) return;
        if (InputManager::instance()->isKeyPressed(31)) {
            mainCamera = 0;
        } else if (InputManager::instance()->isKeyPressed(32)) {
            mainCamera = 1;
        }
#if 1
        if (InputManager::instance()->instance()->isKeyPressed(87)) {
            camera.Position -= glm::vec3(1.0f, 0.0, 0.0) * 4.0f * (float)dt / 1000.0f;
        } else if (InputManager::instance()->instance()->isKeyPressed(83)) {
            camera.Position += glm::vec3(1.0f, 0.0, 0.0) * 4.0f * (float)dt / 1000.0f;
        }

        if (InputManager::instance()->instance()->isKeyPressed(65)) {
            camera.Position += glm::vec3(0.0f, 0.0, 1.0) * 4.0f * (float)dt / 1000.0f;
        } else if (InputManager::instance()->instance()->isKeyPressed(68)) {
            camera.Position -= glm::vec3(0.0f, 0.0, 1.0) * 4.0f * (float)dt / 1000.0f;
        }
#endif
#if 0
		if (InputManager::instance()->instance()->isKeyPressed(69)) {
			double d = 1 - exp(log(0.5) * 130 * (dt * 0.00001));
			float MouseSensitivity = 1.1f;
			float xoffset = (float)InputManager::instance()->mouseMoveX * MouseSensitivity;
			float yoffset = (float)InputManager::instance()->mouseMoveY * MouseSensitivity;
			camera.rotateCamera(xoffset, yoffset);
		}
		

		if (InputManager::instance()->instance()->isKeyPressed(87)) {
			camera.Position -= camera.Front * 4.0f * (float)dt / 1000.0f;
		}
		else if (InputManager::instance()->instance()->isKeyPressed(83)) {
			camera.Position += camera.Front * 4.0f * (float)dt / 1000.0f;
		}

		if (InputManager::instance()->instance()->isKeyPressed(65)) {
			camera.Position += camera.Right * 4.0f * (float)dt / 1000.0f;
		}
		else if (InputManager::instance()->instance()->isKeyPressed(68)) {
			camera.Position -= camera.Right * 4.0f * (float)dt / 1000.0f;
		}
#endif
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

	void onLeftMouseClick(Event* event) {
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

		double x = InputManager::instance()->mousePosX;
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
		}
	}
};