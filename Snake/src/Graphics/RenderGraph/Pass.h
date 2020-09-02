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
#include "Scene/Scene.h"

enum class PassType {
    RENDER_MODELS,
    POSTPROCESS
};

class Pass 
{
protected:
    Viewport viewport;
    std::string debugName;
public:
    std::function<void(Renderer*)> setup;
    std::function<void(Renderer*, Scene*)> execute;
    PassType type;
public:
    Pass(const std::string& name) : debugName(name) {}
};