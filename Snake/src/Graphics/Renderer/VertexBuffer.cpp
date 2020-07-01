#include "VertexBuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/Platform/DirectX/DX11VertexBuffer.h"

VertexBuffer* VertexBuffer::create(int size, int stride, void* data, bool isDynamic) {
    switch (Renderer::getType()) {
        case RendererType::DirectX: return new DX11VertexBuffer(size, stride, data, isDynamic);
    }

    return nullptr;
}