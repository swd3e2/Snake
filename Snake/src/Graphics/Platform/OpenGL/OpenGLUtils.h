#pragma once

#include "Graphics/Renderer/CommonTypes.h"
#include <glad/glad.h>

GLuint getTextureFormat(TextureFormat format);
GLuint getInternalTextureFormat(TextureFormat format);
GLuint getInternalType(TextureFormat format);
GLuint getAddressingMode(const AddressingMode addressingMode);
GLuint getFilteringMode(const FilterMode filteringMode);
GLuint getMinMipFilteringMode(const FilterMode _min, const FilterMode _mip);
GLuint getComparisonFunction(const ComparisonFunction comparisonFunc);