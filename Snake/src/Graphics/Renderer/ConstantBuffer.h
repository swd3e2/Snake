#pragma once

#include <GL/glew.h>
#include "RenderContext.h"

class ConstantBuffer {
private:
	size_t size = 0;
	int position = 0;
public:
	ConstantBuffer(int pos, size_t size) : size(size), position(pos) {}

	const int getPosition() const { return position; }
	const int getSize() const { return size; }

	static ConstantBuffer* create(int pos, size_t size, void* data);

	virtual void update(void* data) = 0;
	virtual void bind(RenderContext* context) = 0;
};