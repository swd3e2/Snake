#pragma once

#include "DirectXRenderContext.h"
#include "Graphics/Renderer/CommonTypes.h"

namespace DirectX {
	const std::string getShaderTarget(const ShaderType type);
	std::string getInputLayoutShaderFormat(InputDataType type);
	DXGI_FORMAT getInputLayoutFormat(InputDataType type);
}