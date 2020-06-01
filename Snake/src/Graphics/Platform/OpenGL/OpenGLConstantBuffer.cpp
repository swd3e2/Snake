#include "OpenGLConstantBuffer.h"

OpenGLConstantBuffer::OpenGLConstantBuffer(int position, size_t size, void* data) :
        ConstantBuffer(position, size)
{
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, this->getSize(), data, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLConstantBuffer::update(void* data) {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, this->getSize(), data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLConstantBuffer::bind(RenderContext* context) {
    glBindBufferBase(GL_UNIFORM_BUFFER, this->getPosition(), ubo);
}