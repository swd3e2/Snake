#include "IndexBuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/Platform/OpenGL/OpenGLIndexBuffer.h"

IndexBuffer* IndexBuffer::create(int size, void* data) {
    switch (Renderer::getType()) {
        case RendererType::OpenGL: return new OpenGLIndexBuffer(size, data);
    }
    return nullptr;
}