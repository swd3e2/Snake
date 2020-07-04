#pragma once

#include "Pass.h"
#include <vector>
#include "Graphics/Renderer.h"
#include <memory>

class RenderGraph {
private:
    std::vector<std::shared_ptr<Pass>> renderPasses;
    Renderer* renderer;
public:
    RenderGraph(Renderer* renderer) :
        renderer(renderer) 
    {}

    void execute() {
        for (auto& it : renderPasses) {
            it->execute(renderer);
        }
    }

    void addPass(const std::shared_ptr<Pass>& renderPass) {
        renderPasses.push_back(renderPass);
    }
};