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
    RenderGraph(Renderer* renderer) : renderer(renderer) {}

    void execute(Scene* scene) {
        for (auto& it : renderPasses) {
            it->setup(renderer);

            if (it->execute) {
                it->execute(renderer, scene);
            }
        }
    }

    void addPass(Pass* renderPass) {
        renderPasses.push_back(renderPass);
    }
};