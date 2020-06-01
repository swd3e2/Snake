#pragma once

#include "Graphics/Renderer/IndexBuffer.h"
#include "OpenGLContext.h"

class OpenGLIndexBuffer : public IndexBuffer {
private:
	unsigned int vbo = 0;
public:
	OpenGLIndexBuffer(int size, void* data);
	virtual void bind(RenderContext* context) override;
};