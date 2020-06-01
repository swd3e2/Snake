#include "ShaderPipeline.h"
#include "Graphics/Renderer.h"
#include "Graphics/Platform/OpenGL/OpenGLShaderPipeline.h"

ShaderPipeline* ShaderPipeline::create(const std::string& vertexShaderCode, const std::string& pixelShaderCode) {
    switch (Renderer::getType())
    {
    case RendererType::OpenGL: return new OpenGLShaderPipeline(vertexShaderCode, pixelShaderCode);
    }
    
    return nullptr;
}
