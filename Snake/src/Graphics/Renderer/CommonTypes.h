#pragma once

enum class TextureFormat {
    R8,
    RG8,
    RGB8,
    BGR8,
    RGBA8,
    BGRA8,
    R16,
    RG16,
    RGB16,
    RGBA16,
    R32F,
	RG32F,
	RG32I,
    RGB32F,
    RGBA32F,
    D24S8,
    D16,
    D32,
    D32_S8X24
};

enum class AddressingMode
{
	WRAP,
	CLAMP,
	MIRROR,
	BORDER
};

enum class FilterMode
{
	NONE,
	POINT,
	LINEAR,
	ANISOTROPIC
};

enum class CullMode
{
	NONE,
	FRONT,
	BACK
};

enum class ComparisonFunction
{
	NEVER,
	LESS,
	EQUAL,
	LEQUAL,
	GREATER,
	NOT_EQUAL,
	ALWAYS
};

enum class FillMode
{
	SOLID,
	WIREFRAME
};

enum InputDataType {
	Float,
	Float2,
	Float3,
	Float4,
	Int,
	Int2,
	Int3,
	Int4,
	Bool
};

enum class ShaderType {
	VERTEX,
	PIXEL,
	GEOMERTY,
	COMPUTE,
	HULL,
	DOMAINS
};

enum TextureFlags {
	TF_None = 0,
	TF_RenderTarget = 1,
	TF_DepthBuffer = 2,
	TF_ShaderResource = 4,
	TF_GenerateMips = 16
};

enum class PrimitiveTopology {
	POINT,
	LINELIST,
	LINESTRIP,
	TRIANGELIST,
	TRIANGESTRIP,
};