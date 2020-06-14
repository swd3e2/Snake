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

enum class PrimitiveTopology
{
	POINT,
	LINELIST,
	LINESTRIP,
	TRIANGELIST,
	TRIANGESTRIP,
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