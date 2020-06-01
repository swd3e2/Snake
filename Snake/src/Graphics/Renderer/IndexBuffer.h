#pragma once

#include <GL/glew.h>
#include "RenderContext.h"

class IndexBuffer {
protected:
	int size = 0;
public:
	IndexBuffer(int size) : size(size) {}

	const int getSize() const { return size; }

	static IndexBuffer* create(int size, void* data);

	virtual void bind(RenderContext* context) = 0;
};