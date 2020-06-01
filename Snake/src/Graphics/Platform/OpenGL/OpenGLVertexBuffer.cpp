#include "OpenGLVertexBuffer.h"

OpenGLVertexBuffer::OpenGLVertexBuffer(int size, int stride, void* data) :
    VertexBuffer(size, stride)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, size * stride, data, 0);

    glBindVertexArray(0);
}

void OpenGLVertexBuffer::bind(RenderContext* context) {
    glBindVertexBuffer(0, vbo, 0, this->getStride());
}