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
    std::queue<std::function<void()>> commands;
    std::string name;
    std::shared_ptr<RenderTarget> renderTarget;
public:
    std::string source;
    std::string sink;
    std::vector<std::shared_ptr<Bindable>> bindables;
    std::unordered_map<int, std::shared_ptr<Texture2D>> textures;
    std::shared_ptr<ShaderPipeline> shader;
public:
    Pass(const std::string& name) : name(name) {}

    virtual void execute(Renderer* renderer) {
        renderTarget->bind(renderer->getContext());

        for (auto& it : bindables) {
            it->bind(renderer->getContext());
        }

		for (auto& it : textures) {
			it.second->bindToUnit(it.first, renderer->getContext());
		}

        this->shader->bind(renderer->getContext());

        while (!commands.empty()) {
            std::function<void()> command = commands.front();
            commands.pop();
            command();
        }
    }
    
	void setRenderTarget(const std::shared_ptr<RenderTarget>& renderTarget) {
		this->renderTarget = renderTarget;
	}

    void setTexture(int slot, const std::shared_ptr<Texture2D>& texture) {
        textures[slot] = texture;
    }
    
    void setShader(const std::shared_ptr<ShaderPipeline>& shader) {
        this->shader = shader;
    }

    virtual void addCommand(std::function<void()> command) {
        commands.push(command);
    }
};