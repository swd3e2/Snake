#include "OpenGLUtils.h"

GLuint getTextureFormat(TextureFormat format) {
    switch (format) {
        case TextureFormat::R8:     return GL_R8;
        case TextureFormat::RG8:    return GL_RG8;
        case TextureFormat::RGB8: 
        case TextureFormat::BGR8:   return GL_RGB8;
        case TextureFormat::RGBA8: 
        case TextureFormat::BGRA8:  return GL_RGBA8;
        case TextureFormat::R16:    return GL_R16;
        case TextureFormat::RG16:   return GL_RG16;
        case TextureFormat::RGB16:  return GL_RGB16;
        case TextureFormat::RGBA16: return GL_RGBA16;
        case TextureFormat::R32:    return GL_R32F;
        case TextureFormat::RG32:   return GL_RG32F;
        case TextureFormat::RGB32:  return GL_RGB32F;
        case TextureFormat::RGBA32: return GL_RGBA32F;
        case TextureFormat::D24S8:  return GL_DEPTH24_STENCIL8;
    }
    
    return GL_RGB8;
}

GLuint getInternalTextureFormat(TextureFormat format)
{
    switch (format) {
        case TextureFormat::R8:
        case TextureFormat::R16:
        case TextureFormat::R32: return GL_RED;
        case TextureFormat::RG8:
        case TextureFormat::RG16:
        case TextureFormat::RG32: return GL_RG;
        case TextureFormat::RGB8:
        case TextureFormat::RGB16:
        case TextureFormat::RGB32: return GL_RGB;
        case TextureFormat::RGBA8:
        case TextureFormat::RGBA16:
        case TextureFormat::RGBA32: return GL_RGBA;
        case TextureFormat::BGRA8: return GL_BGRA;
        case TextureFormat::BGR8: return GL_BGR;
    }
    return GL_RGB;
}