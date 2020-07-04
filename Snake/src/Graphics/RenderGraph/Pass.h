#pragma once

#include <queue>
#include <vector>
#include <string>
#include "Graphics/Renderer.h"
#include "Graphics/Bindable.h"
#include <unordered_map>
#include "Graphics/Renderer/RenderTarget.h"
#include "Graphics/Renderer/Texture2D.h"
#include "Graphics/Renderer/ShaderPipeline.h"

class Pass {
protected:
    std::string name;
    std::shared_ptr<RenderTarget> renderTarget;
public:
    // List of entities to render
	std::queue<entt::entity> entities;

    Viewport viewport;

	std::vector<std::shared_ptr<Bindable>> bindables;
    std::unordered_map<int, std::shared_ptr<Texture2D>> textures;
    std::shared_ptr<ShaderPipeline> shader;
public:
    Pass(const std::string& name) : name(name) {}

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
    }
    
	void setRenderTarget(const std::shared_ptr<RenderTarget>& renderTarget) {
		this->renderTarget = renderTarget;
         viewport = { 0, 0, renderTarget->getWidth(), renderTarget->getHeight() };
	}

    void setTexture(int slot, const std::shared_ptr<Texture2D>& texture) {
        textures[slot] = texture;
    }
    
    void setShader(const std::shared_ptr<ShaderPipeline>& shader) {
        this->shader = shader;
    }

	virtual void addEntity(entt::entity entity) {
		entities.push(entity);
	}
};