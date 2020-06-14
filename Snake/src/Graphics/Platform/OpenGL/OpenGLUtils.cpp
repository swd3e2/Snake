#include "OpenGLUtils.h"

GLuint getTextureFormat(TextureFormat format) {
    switch (format) {
        case TextureFormat::R8:     
            return GL_R8;
        case TextureFormat::RG8:    
            return GL_RG8;
        case TextureFormat::RGB8: 
        case TextureFormat::BGR8:   
            return GL_RGB8;
        case TextureFormat::RGBA8: 
        case TextureFormat::BGRA8:  
            return GL_RGBA8;
        case TextureFormat::R16:    
            return GL_R16;
        case TextureFormat::RG16:   
            return GL_RG16;
        case TextureFormat::RGB16:  
            return GL_RGB16;
        case TextureFormat::RGBA16: 
            return GL_RGBA16;
        case TextureFormat::R32F:    
            return GL_R32F;
		case TextureFormat::RG32I:
			return GL_RG32I;
        case TextureFormat::RG32F:   
            return GL_RG32F;
        case TextureFormat::RGB32F:  
            return GL_RGB32F;
        case TextureFormat::RGBA32F: 
            return GL_RGBA32F;
        case TextureFormat::D24S8:  
            return GL_DEPTH24_STENCIL8;
        case TextureFormat::D16:
			return GL_DEPTH_COMPONENT16;
		case TextureFormat::D32:
			return GL_DEPTH_COMPONENT32F;
        case TextureFormat::D32_S8X24:
            return GL_DEPTH32F_STENCIL8;
    }
    
    return GL_RGB8;
}

GLuint getInternalTextureFormat(TextureFormat format)
{
    switch (format) {
        case TextureFormat::R8:
        case TextureFormat::R16:
        case TextureFormat::R32F: 
            return GL_RED;
        case TextureFormat::RG8:
        case TextureFormat::RG16:
        case TextureFormat::RG32F: 
            return GL_RG;
        case TextureFormat::RGB8:
        case TextureFormat::RGB16:
        case TextureFormat::RGB32F: 
            return GL_RGB;
        case TextureFormat::RGBA8:
        case TextureFormat::RGBA16:
        case TextureFormat::RGBA32F: 
            return GL_RGBA;
        case TextureFormat::BGRA8: 
            return GL_BGRA;
        case TextureFormat::BGR8: 
			return GL_BGR;
		case TextureFormat::RG32I:
			return GL_RG_INTEGER;
		case TextureFormat::D24S8:
		case TextureFormat::D32_S8X24:
			return GL_DEPTH_STENCIL;
		case TextureFormat::D16:
		case TextureFormat::D32:
			return GL_DEPTH_COMPONENT;
    }
    return GL_RGB;
}

GLuint getInternalType(TextureFormat format)
{
    switch (format) {
        case TextureFormat::R8:
        case TextureFormat::RG8:
        case TextureFormat::BGR8:
        case TextureFormat::RGB8:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::RGBA8:
        case TextureFormat::BGRA8:
            return GL_UNSIGNED_INT_8_8_8_8_REV;
        case TextureFormat::R16:
        case TextureFormat::RG16:
        case TextureFormat::RGB16:
        case TextureFormat::RGBA16:
        case TextureFormat::D16:
            return GL_UNSIGNED_SHORT;
        case TextureFormat::R32F:
        case TextureFormat::RG32F:
        case TextureFormat::RGB32F:
        case TextureFormat::RGBA32F:
        case TextureFormat::D32:
			return GL_FLOAT;
		case TextureFormat::RG32I:
            return GL_INT;
		case TextureFormat::D32_S8X24:
			return GL_DEPTH_STENCIL;
    }
    return GL_UNSIGNED_BYTE;
}

GLuint getAddressingMode(const AddressingMode addressingMode)
{
	switch (addressingMode) {
	    case AddressingMode::WRAP:      return GL_REPEAT;
	    case AddressingMode::CLAMP:     return GL_CLAMP_TO_EDGE;
	    case AddressingMode::MIRROR:    return GL_MIRRORED_REPEAT;
	    case AddressingMode::BORDER:    return GL_CLAMP_TO_BORDER;
	}
    return GL_REPEAT;
}

GLuint getFilteringMode(const FilterMode filteringMode)
{
	switch (filteringMode) {
	    case FilterMode::POINT: return GL_NEAREST;
	    case FilterMode::LINEAR: return GL_LINEAR;
	}
	return GL_LINEAR;

}

GLuint getMinMipFilteringMode(const FilterMode _min, const FilterMode _mip)
{
	switch (_min) {
	    case FilterMode::NONE:
	    case FilterMode::POINT:
		    switch (_mip) {
		        case FilterMode::LINEAR:
		        case FilterMode::ANISOTROPIC:   return GL_NEAREST_MIPMAP_LINEAR;
		        case FilterMode::POINT:         return GL_NEAREST_MIPMAP_NEAREST;
		        case FilterMode::NONE:          return GL_NEAREST;
		    }
		    break;
	    case FilterMode::LINEAR:
	    case FilterMode::ANISOTROPIC:
		    switch (_mip) {
		        case FilterMode::LINEAR:
		        case FilterMode::ANISOTROPIC:   return GL_LINEAR_MIPMAP_LINEAR;
		        case FilterMode::POINT:         return GL_LINEAR_MIPMAP_NEAREST;
		        case FilterMode::NONE:          return GL_LINEAR;
		    }
		    break;
	}
}

GLuint getComparisonFunction(const ComparisonFunction comparisonFunc)
{
	switch (comparisonFunc) {
	    case ComparisonFunction::NEVER:     return GL_NEVER;
	    case ComparisonFunction::LESS:      return GL_LESS;
	    case ComparisonFunction::EQUAL:     return GL_EQUAL;
	    case ComparisonFunction::LEQUAL:    return GL_LEQUAL;
	    case ComparisonFunction::GREATER:   return GL_GREATER;
	    case ComparisonFunction::NOT_EQUAL: return GL_NOTEQUAL;
	    case ComparisonFunction::ALWAYS:    return GL_ALWAYS;
	}
	return GL_NEVER;
}