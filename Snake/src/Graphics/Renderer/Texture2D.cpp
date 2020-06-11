#include "Texture2D.h"

#include "Graphics/Renderer.h"
#include "Graphics/Platform/OpenGL/OpenGLTexture2D.h"

Texture2D* Texture2D::create(int width, int height, int location, void* data, TextureFormat textureFormat, int numMips) {
    switch (Renderer::getType()) {
        case RendererType::OpenGL: return new OpenGLTexture2D(width, height, location, data, textureFormat, numMips);
    }

    return nullptr;
}