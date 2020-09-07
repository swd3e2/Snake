#include "DepthState.h"
#include "Graphics/Renderer.h"
#include "Graphics/Platform/DirectX/DX11DepthState.h"

DepthState* DepthState::create(const DepthStateDesc& desc)
{
    switch (Renderer::getType()) {
        case RendererType::DirectX: return new DX11DepthState(desc);
    }
    return nullptr;
}