#pragma once

#include "Pass.h"
#include <vector>
#include "Graphics/Renderer.h"
#include "Model/Model.h"
#include <memory>

class RenderGraph {
private:
    std::vector<Pass*> renderPasses;
    Renderer* renderer;
public:
    std::array<Model*, 10000> models;

    RenderGraph(Renderer* renderer) :
        renderer(renderer) 
    {
        for (auto& model : models) {
            model = nullptr;
        }
    }

    void execute() {
        for (auto& it : renderPasses) {
            it->setupPass(renderer);

            if (it->type == PassType::RENDER_MODELS) {
                for (auto& model : models) {
                    if (model == nullptr) break;
                    it->processModel(renderer, model);
                }
            } else if (it->execute) {
                it->execute(renderer);
            }
        }
    }

    void addPass(Pass* renderPass) {
        renderPasses.push_back(renderPass);
    }
};