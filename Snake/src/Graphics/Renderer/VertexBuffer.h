#pragma once

#include <GL/glew.h>
#include "RenderContext.h"

class VertexBuffer {
private:
	int stride = 0;
	int size = 0;
public:
	VertexBuffer(int size, int stride) : stride(stride), size(size) {}
	int getSize() const { return size; }
	int getStride() const { return stride; }

	static VertexBuffer* create(int size, int stride, void* data);
	
	virtual void bind(RenderContext* context) = 0;
};