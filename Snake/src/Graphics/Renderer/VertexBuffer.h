#pragma once

#include "RenderContext.h"
#include "Graphics/Bindable.h"

class VertexBuffer : public Bindable {
protected:
	int stride = 0;
	int size = 0;
	bool isDynamic = false;
public:
	VertexBuffer(int size, int stride, bool isDynamic) : stride(stride), size(size), isDynamic(isDynamic) {}
	virtual ~VertexBuffer() {}
	const int getSize() const { return size; }
	const int getStride() const { return stride; }
	virtual void update(void* data, size_t size, size_t stride) = 0;

	static VertexBuffer* create(int size, int stride, void* data, bool isDynamic = false);
};