#include "RenderTarget.h"
#include "Graphics/Renderer.h"
#include "Graphics/Platform/DirectX/DX11RenderTarget.h"

RenderTarget* RenderTarget::create() {
    switch (Renderer::getType()) {
        case RendererType::DirectX: return new DX11RenderTarget();
    }
    return nullptr;
}