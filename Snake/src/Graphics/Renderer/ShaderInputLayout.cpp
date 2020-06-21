#include "ShaderInputLayout.h"
#include "Graphics/Renderer.h"
#include "Graphics/Platform/DirectX/DX11ShaderInputLayout.h"

ShaderInputLayout* ShaderInputLayout::create(const ShaderInputLayoutDesc& desc) {
	switch (Renderer::getType()) {
		case RendererType::DirectX:		return new DX11ShaderInputLayout(desc);
	}
	return nullptr;
}
