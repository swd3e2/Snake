#include "OpenGLIndexBuffer.h"

OpenGLIndexBuffer::OpenGLIndexBuffer(int size, void* data) : 
    IndexBuffer(size)
{
    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, size * 4, data, 0);
}

void OpenGLIndexBuffer::bind(RenderContext* context) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
}