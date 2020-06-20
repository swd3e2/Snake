#include "ShaderInputLayout.h"
#include "Graphics/Renderer.h"
#include "Graphics/Platform/OpenGL/OpenGLShaderInputLayout.h"
#include "Graphics/Platform/DirectX/DirectXShaderInputLayout.h"

ShaderInputLayout* ShaderInputLayout::create(const ShaderInputLayoutDesc& desc) {
	switch (Renderer::getType()) {
		case RendererType::OpenGL:		return new OpenGLShaderInputLayout(desc);
		case RendererType::DirectX:		return new DirectXShaderInputLayout(desc);
	}
	return nullptr;
}
