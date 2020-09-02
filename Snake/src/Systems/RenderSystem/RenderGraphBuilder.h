#pragma once

#include "Graphics/RenderGraph/RenderGraph.h"
#include "RenderSystemResource.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderQueue/RenderQueue.h"

class RenderGraphBuilder {
public:
	RenderGraph* build(RenderQueue* renderQueue, Renderer* renderer, RenderSystemResource* resource)
	{
		RenderGraph* renderGraph = new RenderGraph(renderer);

		{
			Pass* clearRenderTargetPass = new Pass("ClearRenderTargetPass");
			clearRenderTargetPass->type = PassType::POSTPROCESS;

			clearRenderTargetPass->setup = [
				renderQueue = renderQueue, 
				resource = resource
			] (Renderer* renderer) {
				RenderContext* context = renderer->getContext();

				renderQueue->clearRenderTarget(resource->main_render_target);
				for (auto& it : resource->renderTargets) {
					renderQueue->clearRenderTarget(it.second.get());
				}
			};
			renderGraph->addPass(clearRenderTargetPass);
		}
		{
			Pass* gbufferPass = new Pass("GBufferPass");
			gbufferPass->type = PassType::RENDER_MODELS;

			gbufferPass->setup = [renderQueue = renderQueue,resource = resource](Renderer* renderer) {
				RenderContext* context = renderer->getContext();

				renderQueue->bindConstantBuffer(resource->shader_buffer.get(), 0);
				renderQueue->bindShader(resource->shaders["gbuffer"].get());
				renderQueue->bindRenderTarget(resource->renderTargets["gBuffer"].get());
			};

			gbufferPass->execute = [
				renderQueue = renderQueue,
				resource = resource
			] (Renderer* renderer, Scene* scene) {
				ModelData modelData;
				MeshData meshData;

				auto group = scene->getRegistry()->group<RenderComponent, TransformComponent>();

				for (auto entity : group) {
					auto& transform = group.get<TransformComponent>(entity);
					auto& render = group.get<RenderComponent>(entity);
					auto& model = render.model;

					modelData.toWorld = glm::transpose(transform.matrix);
					modelData.inverseToWorld = glm::inverse(modelData.toWorld);
					modelData.selected = false;

					renderQueue->updateConstantBuffer(resource->model_shader_buffer.get(), &modelData, sizeof(ModelData));
					renderQueue->bindConstantBuffer(resource->model_shader_buffer.get(), resource->model_shader_buffer->getPosition());

					std::unordered_map<int, std::vector<std::shared_ptr<SubMesh>>>& submeshes = model->submeshes;
					const std::vector<std::shared_ptr<Material>>& materials = model->materials;

					for (auto& node : model->nodes) {
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
							}
							else {
								meshData.materialData.diffuseColor = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
								meshData.materialData.hasDiffuseMap = false;
								meshData.materialData.hasNormalMap = false;
							}

							meshData.transform = glm::transpose(node->transform.worldTransform);
							meshData.normalTransform = glm::transpose(node->transform.normalTransform);

							/*meshData.transform = glm::mat4(1.0f);
							meshData.normalTransform = glm::mat4(1.0f);*/

							renderQueue->updateConstantBuffer(resource->mesh_shader_buffer.get(), &meshData, sizeof(MeshData));
							renderQueue->bindConstantBuffer(resource->mesh_shader_buffer.get(), resource->mesh_shader_buffer->getPosition());

							renderQueue->bindVertexBuffer(submesh->vBuffer.get());
							renderQueue->bindIndexBuffer(submesh->iBuffer.get());

							renderQueue->drawIndexed(submesh->iBuffer->getSize());
						}
					};
				}
			};

			renderGraph->addPass(gbufferPass);
		}
		//{
		//	PhysicDebugRenderPass* debugPhysicsPass = new PhysicDebugRenderPass("Debug Physics Pass");

		//	debugPhysicsPass->setRenderTarget(resource->renderTargets["gBuffer"]);
		//	debugPhysicsPass->setShader(resource->shaders["physdebug"]);
		//	debugPhysicsPass->setConstantBuffer(resource->shaderBuffer);

		//	renderGraph->addPass(debugPhysicsPass);
		//}
		//{
		//	Pass* pickerPass = new Pass("Picker pass");

		//	pickerPass->setup = [resource = resource](Renderer* renderer) {
		//		RenderContext* context = renderer->getContext();

		//		resource->shaderBuffer->bind(context);
		//		resource->shaders["picker"]->bind(context);
		//		resource->renderTargets["picker"]->bind(context);
		//	};
		//	pickerPass->renderModels = [resource = resource](Renderer* renderer) {
		//		RenderContext* context = renderer->getContext();

		//		ModelData modelData;

		//		modelData.toWorld = glm::transpose(transform.matrix);
		//		modelData.inverseToWorld = glm::inverse(modelData.toWorld);
		//		resource->modelShaderBuffer->update((void*)&modelData);
		//		resource->modelShaderBuffer->bind(renderer->getContext());

		//		glm::vec4 color;
		//		color.r = (((int)entity & (0x000000FF)) >> 0) / 255.0f;
		//		color.g = (((int)entity & (0x0000FF00)) >> 8) / 255.0f;
		//		color.b = (((int)entity & (0x00FF0000)) >> 16) / 255.0f;
		//		color.a = 1.0f;

		//		resource->pickerBuffer->update((void*)&color);
		//		resource->pickerBuffer->bind(renderer->getContext());

		//		auto& submeshes = render.model->submeshes;
		//		auto& materials = render.model->materials;

		//		for (auto& node : render.model->nodes) {
		//			if (node->mesh == -1) continue;

		//			const std::vector<std::shared_ptr<SubMesh>>& submeshs = submeshes[node->mesh];

		//			for (auto& submesh : submeshs) {
		//				MeshData meshData;
		//				meshData.transform = glm::transpose(node->transform.worldTransform);
		//				meshData.normalTransform = glm::transpose(node->transform.normalTransform);

		//				resource->meshShaderBuffer->update(&meshData);
		//				resource->meshShaderBuffer->bind(renderer->getContext());

		//				submesh->vBuffer->bind(renderer->getContext());
		//				submesh->iBuffer->bind(renderer->getContext());

		//				renderer->drawIndexed(submesh->iBuffer->getSize());
		//			}
		//		};
		//	};

		//	renderGraph->addPass(pickerPass);
		//}
		////{
		////	shadowPass.reset(new Pass("Shadow Pass"));
		////	shadowPass->setRenderTarget(renderTargets["shadow"]); 
		////	shadowPass->setShader(shaders["shadow"]);

		////	renderGraph->addPass(shadowPass);
		////}
		////{
		////	blurPass.reset(new FullscreenPass("Gaussian Blur Pass"));
		////	blurPass->setRenderTarget(renderTargets["blur"]);
		////	blurPass->setShader(shaders["blur"]);
		////	blurPass->setTexture(0, textures["shadowColorTexture"]);

		////	renderGraph->addPass(blurPass);
		////}
		//{
		//	FullscreenPass* aoPass = new FullscreenPass("Ambient Occlusion Pass");

		//	aoPass->setRenderTarget(resource->renderTargets["ambient_occlusion"]);

		//	aoPass->setShader(resource->shaders["ambient_occlusion"]);
		//	aoPass->setTexture(0, resource->textures["view_space_pos"]);
		//	aoPass->setTexture(1, resource->textures["normals"]);
		//	aoPass->setTexture(2, resource->textures["view_space_normal"]);
		//	aoPass->setTexture(3, resource->textures["noise"]);
		//	aoPass->setTexture(4, resource->textures["g_buffer_depth"]);
		//	aoPass->setConstantBuffer(resource->ambientOcclusionBuffer);

		//	renderGraph->addPass(aoPass);
		//}
		//{
		//	FullscreenPass* aoBlurPass = new FullscreenPass("Ambient occluseion blur pass");
		//	aoBlurPass->setRenderTarget(resource->renderTargets["blur"]);
		//	aoBlurPass->setShader(resource->shaders["blur"]);
		//	aoBlurPass->setTexture(0, resource->textures["ambient_occlusion_color"]);

		//	renderGraph->addPass(aoBlurPass);
		//}
		//{
		//	FullscreenPass* lightGBufferPass = new FullscreenPass("Light Pass");

		//	lightGBufferPass->setRenderTarget(resource->renderTargets["light"]);

		//	lightGBufferPass->setConstantBuffer(resource->shaderBuffer);
		//	lightGBufferPass->setConstantBuffer(resource->modelShaderBuffer);
		//	lightGBufferPass->setConstantBuffer(resource->meshShaderBuffer);

		//	lightGBufferPass->setShader(resource->shaders["light"]);
		//	lightGBufferPass->setTexture(0, resource->textures["positions"]);
		//	lightGBufferPass->setTexture(1, resource->textures["normals"]);
		//	lightGBufferPass->setTexture(2, resource->textures["diffuse"]);
		//	lightGBufferPass->setTexture(3, resource->textures["bluredShadowDepthTexture"]);
		//	lightGBufferPass->setTexture(4, resource->textures["g_buffer_depth"]);

		//	renderGraph->addPass(lightGBufferPass);
		//}
		{
			Pass* mainRenderPass = new Pass("Result Pass");
			mainRenderPass->type = PassType::POSTPROCESS;

			mainRenderPass->setup = [
				renderQueue = renderQueue,
				resource = resource
			] (Renderer* renderer) {
				RenderContext* context = renderer->getContext();
				// @todo: fix render target binding
				renderQueue->bindRenderTarget(resource->main_render_target);

				renderQueue->bindTexture(resource->textures["positions"].get(), 0);
				renderQueue->bindTexture(resource->textures["normals"].get(), 1);
				renderQueue->bindTexture(resource->textures["diffuse"].get(), 2);
				renderQueue->bindShader(resource->shaders["default"].get());
			};
			
			mainRenderPass->execute = [
				renderQueue = renderQueue,
				resource = resource
			] (Renderer* renderer, Scene* scene) {
				RenderContext* context = renderer->getContext();

				renderQueue->bindVertexBuffer(resource->quadVertexBuffer.get());
				renderQueue->bindIndexBuffer(resource->quadIndexBuffer.get());

				renderQueue->drawIndexed(resource->quadIndexBuffer->getSize());
			};
			renderGraph->addPass(mainRenderPass);
		}

		return renderGraph;
	}
};