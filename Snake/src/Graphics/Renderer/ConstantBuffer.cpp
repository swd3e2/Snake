#include "ConstantBuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/Platform/DirectX/DX11ConstantBuffer.h"

ConstantBuffer* ConstantBuffer::create(int pos, size_t size, void* data) {
    switch (Renderer::getType()) {
        case RendererType::DirectX: return new DX11ConstantBuffer(pos, size, data);
    }
    return nullptr;
}