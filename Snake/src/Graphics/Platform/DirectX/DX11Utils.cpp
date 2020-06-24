#include "DX11Utils.h"

namespace DirectX {
	const std::string getShaderTarget(const ShaderType type) {
		switch (type) {
			case ShaderType::VERTEX:	return "vs_5_0";
			case ShaderType::PIXEL:		return "ps_5_0";
			case ShaderType::GEOMERTY:	return "gs_5_0";
			case ShaderType::COMPUTE:	return "cs_5_0";
			case ShaderType::HULL:		return "hs_5_0";
			case ShaderType::DOMAINS:	return "ds_5_0";
		}
		return "vs_5_0";
	}

	std::string getInputLayoutShaderFormat(InputDataType type) {
		switch (type) {
			case InputDataType::Float:		return "float";
			case InputDataType::Float2:		return "float2";
			case InputDataType::Float3:		return "float3";
			case InputDataType::Float4:		return "float4";
			case InputDataType::Int:		return "int";
			case InputDataType::Int2:		return "int2";
			case InputDataType::Int3:		return "int3";
			case InputDataType::Int4:		return "int4";
			case InputDataType::Bool:		return "bool";
		}
		return "";
	}

	DXGI_FORMAT getInputLayoutFormat(InputDataType type) {
		switch (type) {
			case InputDataType::Float:		return DXGI_FORMAT_R32_FLOAT;
			case InputDataType::Float2:		return DXGI_FORMAT_R32G32_FLOAT;
			case InputDataType::Float3:		return DXGI_FORMAT_R32G32B32_FLOAT;
			case InputDataType::Float4:		return DXGI_FORMAT_R32G32B32A32_FLOAT;
			case InputDataType::Int:		return DXGI_FORMAT_R32_SINT;
			case InputDataType::Int2:		return DXGI_FORMAT_R32G32_SINT;
			case InputDataType::Int3:		return DXGI_FORMAT_R32G32B32_SINT;
			case InputDataType::Int4:		return DXGI_FORMAT_R32G32B32A32_SINT;
			case InputDataType::Bool:		return DXGI_FORMAT_R32_UINT;
		}
	}

	DXGI_FORMAT getTextureFormat(TextureFormat textureFormat) {
		switch (textureFormat) {
			case TextureFormat::R8:				return DXGI_FORMAT_R8_UNORM;
			case TextureFormat::RG8:			return DXGI_FORMAT_R8G8_UNORM;
			case TextureFormat::RGB8:			
			case TextureFormat::RGBA8:			return DXGI_FORMAT_R8G8B8A8_UNORM;
			case TextureFormat::BGR8:			return DXGI_FORMAT_B8G8R8X8_UNORM;
			case TextureFormat::BGRA8:			return DXGI_FORMAT_B8G8R8A8_UNORM;
			case TextureFormat::R16:			return DXGI_FORMAT_R16_UNORM;
			case TextureFormat::RG16:			return DXGI_FORMAT_R16G16_UNORM;
			case TextureFormat::RGB16:
			case TextureFormat::RGBA16:			return DXGI_FORMAT_R16G16B16A16_UNORM;
			case TextureFormat::R32F:			return DXGI_FORMAT_R32_FLOAT;
			case TextureFormat::RG32F:			return DXGI_FORMAT_R32G32_FLOAT;
			case TextureFormat::RGB32F:			return DXGI_FORMAT_R32G32B32_FLOAT;
			case TextureFormat::RGBA32F:		return DXGI_FORMAT_R32G32B32A32_FLOAT;
			case TextureFormat::D24S8:			return DXGI_FORMAT_R24G8_TYPELESS;
			case TextureFormat::D32:			return DXGI_FORMAT_R32_TYPELESS;
		}
	}

	const unsigned int getPitch(TextureFormat textureFormat, int width) {
		switch (textureFormat) {
			case TextureFormat::R8:
			case TextureFormat::R16:
			case TextureFormat::R32F:
				return 1 * width;
			case TextureFormat::RG8:
			case TextureFormat::RG16:
			case TextureFormat::RG32F:
				return 2 * width;
			case TextureFormat::RGB8:
			case TextureFormat::BGR8:
			case TextureFormat::RGB16:
			case TextureFormat::RGB32F:
				return 3 * width;
			case TextureFormat::RGBA8:
			case TextureFormat::BGRA8:
			case TextureFormat::RGBA16:
			case TextureFormat::RGBA32F:
				return 4 * width;
		}
	}

	D3D11_TEXTURE_ADDRESS_MODE getAddressingMode(const AddressingMode& mode) {
		switch (mode) {
			case AddressingMode::WRAP: return  D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			case AddressingMode::CLAMP: return  D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
			case AddressingMode::MIRROR: return  D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_MIRROR;
			case AddressingMode::BORDER: return  D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_BORDER;
		}
		return D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	}

	D3D11_COMPARISON_FUNC getComparisonFunction(const ComparisonFunction& func) {
		switch (func) {
			case ComparisonFunction::NEVER:		return D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
			case ComparisonFunction::LESS:		return D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
			case ComparisonFunction::EQUAL:		return D3D11_COMPARISON_FUNC::D3D11_COMPARISON_EQUAL;
			case ComparisonFunction::LEQUAL:	return D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
			case ComparisonFunction::GREATER:	return D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER;
			case ComparisonFunction::NOT_EQUAL:	return D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NOT_EQUAL;
			case ComparisonFunction::ALWAYS:	return D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
		}
		return D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
	}

	D3D11_FILTER getFilteringMode(const FilterMode& _min, const FilterMode& mag, const FilterMode& mip, const bool comparison)
	{
#define MERGE_FILTERS(_comparison, _min, _mag, _mip) ((_comparison << 16) | (_min << 8) | (_mag << 4) | (_mip))
		switch ((MERGE_FILTERS((int)comparison, (int)_min, (int)mag, (int)mip))) {
		case MERGE_FILTERS(true, (int)(int)FilterMode::POINT, (int)(int)FilterMode::POINT, (int)(int)FilterMode::POINT):
			return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		case MERGE_FILTERS(true, (int)FilterMode::POINT, (int)FilterMode::POINT, (int)FilterMode::LINEAR):
			return D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
		case MERGE_FILTERS(true, (int)FilterMode::POINT, (int)FilterMode::LINEAR, (int)FilterMode::POINT):
			return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
		case MERGE_FILTERS(true, (int)FilterMode::POINT, (int)FilterMode::LINEAR, (int)FilterMode::LINEAR):
			return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
		case MERGE_FILTERS(true, (int)FilterMode::LINEAR, (int)FilterMode::POINT, (int)FilterMode::POINT):
			return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
		case MERGE_FILTERS(true, (int)FilterMode::LINEAR, (int)FilterMode::POINT, (int)FilterMode::LINEAR):
			return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		case MERGE_FILTERS(true, (int)FilterMode::LINEAR, (int)FilterMode::LINEAR, (int)FilterMode::POINT):
			return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		case MERGE_FILTERS(true, (int)FilterMode::LINEAR, (int)FilterMode::LINEAR, (int)FilterMode::LINEAR):
			return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		case MERGE_FILTERS(true, (int)FilterMode::ANISOTROPIC, (int)FilterMode::ANISOTROPIC, (int)FilterMode::ANISOTROPIC):
			return D3D11_FILTER_COMPARISON_ANISOTROPIC;
		case MERGE_FILTERS(false, (int)FilterMode::POINT, (int)FilterMode::POINT, (int)FilterMode::POINT):
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		case MERGE_FILTERS(false, (int)FilterMode::POINT, (int)FilterMode::POINT, (int)FilterMode::LINEAR):
			return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		case MERGE_FILTERS(false, (int)FilterMode::POINT, (int)FilterMode::LINEAR, (int)FilterMode::POINT):
			return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		case MERGE_FILTERS(false, (int)FilterMode::POINT, (int)FilterMode::LINEAR, (int)FilterMode::LINEAR):
			return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		case MERGE_FILTERS(false, (int)FilterMode::LINEAR, (int)FilterMode::POINT, (int)FilterMode::POINT):
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		case MERGE_FILTERS(false, (int)FilterMode::LINEAR, (int)FilterMode::POINT, (int)FilterMode::LINEAR):
			return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		case MERGE_FILTERS(false, (int)FilterMode::LINEAR, (int)FilterMode::LINEAR, (int)FilterMode::POINT):
			return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		case MERGE_FILTERS(false, (int)FilterMode::LINEAR, (int)FilterMode::LINEAR, (int)FilterMode::LINEAR):
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		case MERGE_FILTERS(false, (int)FilterMode::ANISOTROPIC, (int)FilterMode::ANISOTROPIC, (int)FilterMode::ANISOTROPIC):
			return D3D11_FILTER_ANISOTROPIC;
		default:
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		}
#undef MERGE_FILTERS
	}

	DXGI_FORMAT getDepthResourceFormat(TextureFormat textureFormat) {
		switch (textureFormat) {
			case TextureFormat::D16:				return DXGI_FORMAT::DXGI_FORMAT_D16_UNORM;
			case TextureFormat::D24S8:				return DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
			case TextureFormat::D32:				return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
			case TextureFormat::D32_S8X24:			return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		}
		return DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
	}
}