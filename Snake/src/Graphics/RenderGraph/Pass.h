#pragma once

#include <queue>
#include <string>
#include <vector>
#include "Graphics/Renderer.h"
#include <unordered_map>
#include "Graphics/Renderer/RenderTarget.h"
#include "Graphics/Renderer/Texture2D.h"
#include "Graphics/Renderer/ShaderPipeline.h"
#include <functional>

enum class PassType {
    RENDER_MODELS,
    FULLSCREEN
};

class Pass 
{
protected:
    Viewport viewport;
    std::string debugName;
public:
    std::function<void(Renderer*)> setupPass;
    std::function<void(Renderer*, Model*)> processModel;
    std::function<void(Renderer*)> execute;
    bool renderModels = false;
    PassType type;
public:
    Pass(const std::string& name) : debugName(name) {}
};