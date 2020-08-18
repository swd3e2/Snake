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
	
public:
	//void (*func)(Renderer* renderer, entt::entity entity);
	std::function<void(Renderer*, entt::entity)> func;
	std::function<void()> func2;
private:
	entt::registry* registry;
	//ModelData modelData;
	//MeshData meshData;

	std::shared_ptr<ConstantBuffer> modelShaderBuffer;
	std::shared_ptr<ConstantBuffer> meshShaderBuffer;
public:
	DefaultRenderPass(const std::string& name, entt::registry* registry) :
		Pass(name), registry(registry)
	{
		//modelShaderBuffer.reset(ConstantBuffer::create(1, sizeof(ModelData), nullptr));
		//meshShaderBuffer.reset(ConstantBuffer::create(2, sizeof(MeshData), nullptr));
	}

	virtual void execute(Renderer* renderer) {
		/*renderer->setViewport(viewport);
		renderTarget->bind(renderer->getContext());

		for (auto& it : textures) {
			it.second->bindToUnit(it.first, renderer->getContext());
		}
		for (auto& it : buffers) {
			it->bind(renderer->getContext());
		}
		this->shader->bind(renderer->getContext());

		if (func2) {
			func2();
		}

		while (!entities.empty()) {
			entt::entity entity = entities.front();
			entities.pop();
			renderEntity(renderer, entity);
		}*/
	}
private:
	void renderEntity(Renderer* renderer, entt::entity entity) {
		func(renderer, entity);
	}
};