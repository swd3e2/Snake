#pragma once

#include <glad/glad.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include "RenderContext.h"
#include "Graphics/Bindable.h"

class ShaderPipeline : public Bindable
{
public:
    ShaderPipeline(const std::string& vertexShaderCode, const std::string& pixelShaderCode) {}
    static ShaderPipeline* create(const std::string& vertexShaderCode, const std::string& pixelShaderCode);
};