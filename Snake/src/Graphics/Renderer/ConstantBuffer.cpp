#include "ConstantBuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/Platform/OpenGL/OpenGLConstantBuffer.h"

ConstantBuffer* ConstantBuffer::create(int pos, size_t size, void* data) {
    switch (Renderer::getType())
    {
    case RendererType::OpenGL: return new OpenGLConstantBuffer(pos, size, data);
    }

    return nullptr;
}