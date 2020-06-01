#include "VertexBuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/Platform/OpenGL/OpenGLVertexBuffer.h"

VertexBuffer* VertexBuffer::create(int size, int stride, void* data) {
    switch (Renderer::getType())
    {
    case RendererType::OpenGL:
        return new OpenGLVertexBuffer(size, stride, data);
        break;
    }

    return nullptr;
}