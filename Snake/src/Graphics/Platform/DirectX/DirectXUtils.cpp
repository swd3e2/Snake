#include "DirectXUtils.h"

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
}