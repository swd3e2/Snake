#include "IndexBuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/Platform/DirectX/DX11IndexBuffer.h"

IndexBuffer* IndexBuffer::create(int size, void* data) {
    switch (Renderer::getType()) {
        case RendererType::DirectX: return new DX11IndexBuffer(size, data);
    }
    return nullptr;
}