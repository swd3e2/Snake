#pragma once

#include "Pass.h"
#include "Components.h"
#include <entt/entt.hpp>
#include "Graphics/Renderer/ConstantBuffer.h"
#include "CommonRenderTypes.h"

/**
 * Class for default rendering using material data.
 */
class DefaultRenderPass : public Pass {
private:
	entt::registry* registry;
	ModelData modelData;

	std::shared_ptr<ConstantBuffer> modelShaderBuffer;
	std::shared_ptr<ConstantBuffer> meshShaderBuffer;
public:
	DefaultRenderPass(const std::string& name, entt::registry* registry) :
		Pass(name), registry(registry)
	{
		modelShaderBuffer.reset(ConstantBuffer::create(1, sizeof(ModelData), nullptr));
		meshShaderBuffer.reset(ConstantBuffer::create(2, sizeof(MeshData), nullptr));
	}

	virtual void execute(Renderer* renderer) {
		renderer->setViewport(viewport);
		renderTarget->bind(renderer->getContext());

		for (auto& it : bindables) {
			it->bind(renderer->getContext());
		}

		for (auto& it : textures) {
			it.second->bindToUnit(it.first, renderer->getContext());
		}

		this->shader->bind(renderer->getContext());

		while (!entities.empty()) {
			entt::entity entity = entities.front();
			entities.pop();
			renderEntity(renderer, entity);
		}
	}
private:
	void renderEntity(Renderer* renderer, entt::entity entity) {
		Transform& transform = registry->get<Transform>(entity);
		Render& render = registry->get<Render>(entity);

		modelData.toWorld = glm::transpose(transform.matrix);
		modelData.inverseToWorld = glm::inverse(modelData.toWorld);
		modelShaderBuffer->update((void*)&modelData);
		modelShaderBuffer->bind(renderer->getContext());

		const std::vector<std::shared_ptr<Model::SubMesh>>& submeshes = render.model->getSubMeshes();
		const std::vector<std::shared_ptr<Material>>& materials = render.model->getMaterials();

		for (auto& node : render.model->getNodes()) {
			if (node->mesh == -1) continue;

			const std::shared_ptr<Model::SubMesh>& submesh = submeshes[node->mesh];
			const std::shared_ptr<Material>& material = materials[submesh->material];

			MeshData meshData;

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

			meshData.transform = glm::transpose(node->transform.worldTransform);
			meshData.normalTransform = glm::transpose(node->transform.normalTransform);

			meshShaderBuffer->update(&meshData);
			meshShaderBuffer->bind(renderer->getContext());

			submesh->vBuffer->bind(renderer->getContext());
			submesh->iBuffer->bind(renderer->getContext());

			renderer->drawIndexed(submesh->iBuffer->getSize());
		};
	}
};