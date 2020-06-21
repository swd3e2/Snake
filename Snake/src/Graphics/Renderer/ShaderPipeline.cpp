#include "ShaderPipeline.h"
#include "Graphics/Renderer.h"
#include "Graphics/Platform/DirectX/DX11ShaderPipeline.h"

ShaderPipeline* ShaderPipeline::create(const std::string& vertexShaderCode, const std::string& pixelShaderCode) {
    switch (Renderer::getType()) {
	    case RendererType::DirectX:     return new DX11ShaderPipeline(vertexShaderCode, pixelShaderCode);
    }
    return nullptr;
}
