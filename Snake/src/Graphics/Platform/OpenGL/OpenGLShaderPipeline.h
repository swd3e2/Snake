#pragma once

#include "Graphics/Renderer/ShaderPipeline.h"
#include "Common/Helper.h"
#include <string>

class OpenGLShaderPipeline : public ShaderPipeline {
public:
	unsigned int vertexShaderId;
	unsigned int pixelShaderId;
	unsigned int shaderPipelineId;
public:
    OpenGLShaderPipeline(const std::string& vertexShaderCode, const std::string& pixelShaderCode);
    virtual void bind(RenderContext* context) override;
private:
	bool checkErrors(GLuint shaderId);
	void activatePixelShaderTextureSlots(const std::string& pixelShaderCode, GLuint pixelShaderId);
};