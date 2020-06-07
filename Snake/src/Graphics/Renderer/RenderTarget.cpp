#include "RenderTarget.h"
#include "Graphics/Renderer.h"
#include "Graphics/Platform/OpenGL/OpenGLRenderTarget.h"

RenderTarget* RenderTarget::create() {
    switch (Renderer::getType()) {
        case RendererType::OpenGL: return new OpenGLRenderTarget();
    }
    return nullptr;
}