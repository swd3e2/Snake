#pragma once

#include "DX11RenderContext.h"
#include "Graphics/Renderer/CommonTypes.h"

namespace DirectX {
	const std::string getShaderTarget(const ShaderType type);
	std::string getInputLayoutShaderFormat(InputDataType type);
	DXGI_FORMAT getInputLayoutFormat(InputDataType type);
	DXGI_FORMAT getTextureFormat(TextureFormat textureFormat);
	const unsigned int getPitch(TextureFormat textureFormat, int width);
	D3D11_TEXTURE_ADDRESS_MODE getAddressingMode(const AddressingMode& mode);
	D3D11_COMPARISON_FUNC getComparisonFunction(const ComparisonFunction& func);
	D3D11_FILTER getFilteringMode(const FilterMode& _min, const FilterMode& mag, const FilterMode& mip, const bool comparison);
	DXGI_FORMAT getDepthResourceFormat(TextureFormat textureFormat);
	D3D_PRIMITIVE_TOPOLOGY getTopology(PrimitiveTopology topology);
}