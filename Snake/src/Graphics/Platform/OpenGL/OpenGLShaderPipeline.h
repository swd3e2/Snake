#pragma once

#include "Graphics/Renderer/ShaderPipeline.h"

class OpenGLShaderPipeline : public ShaderPipeline {
public:
	unsigned int vertexShaderId;
	unsigned int pixelShaderId;
	unsigned int shaderPipelineId;
public:
    OpenGLShaderPipeline(const std::string& vertexShaderCode, const std::string& pixelShaderCode);
    virtual void bind(RenderContext* context) override;
};