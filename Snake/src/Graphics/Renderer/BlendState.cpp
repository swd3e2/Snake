#include "BlendState.h"
#include "Graphics/Renderer.h"
#include "Graphics/Platform/DirectX/DX11BlendState.h"

BlendState* BlendState::create(const BlendStateDesc& desc)
{
    switch (Renderer::getType()) {
        case RendererType::DirectX: return new DX11BlendState(desc);
    }
    return nullptr;
}