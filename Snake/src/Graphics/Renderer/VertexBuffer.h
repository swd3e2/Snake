#pragma once

#include <glad/glad.h>
#include "RenderContext.h"
#include "Graphics/Bindable.h"

class VertexBuffer : public Bindable {
private:
	int stride = 0;
	int size = 0;
public:
	VertexBuffer(int size, int stride) : stride(stride), size(size) {}
	int getSize() const { return size; }
	int getStride() const { return stride; }

	static VertexBuffer* create(int size, int stride, void* data);
};