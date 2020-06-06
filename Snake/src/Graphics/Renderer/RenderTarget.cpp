#include "RenderTarget.h"
#include "Graphics/Renderer.h"
#include "Graphics/Platform/OpenGL/OpenGLRenderTarget.h"

RenderTarget* RenderTarget::create(int width, int height, int cnt, TextureFormat colorTextureFormat) {
    switch (Renderer::getType()) {
        case RendererType::OpenGL: return new OpenGLRenderTarget(width, height, cnt, colorTextureFormat);
    }
    return nullptr;
}