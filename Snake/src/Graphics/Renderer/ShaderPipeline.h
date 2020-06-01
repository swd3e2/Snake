#pragma once

#include <GL/glew.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include "RenderContext.h"

class ShaderPipeline
{
public:
    ShaderPipeline(const std::string& vertexShaderCode, const std::string& pixelShaderCode) {}
    static ShaderPipeline* create(const std::string& vertexShaderCode, const std::string& pixelShaderCode);

    virtual void bind(RenderContext* context) = 0;
};