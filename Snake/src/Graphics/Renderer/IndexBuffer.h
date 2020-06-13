#pragma once

#include <glad/glad.h>
#include "RenderContext.h"
#include "Graphics/Bindable.h"

class IndexBuffer : public Bindable {
protected:
	int size = 0;
public:
	IndexBuffer(int size) : size(size) {}

	const int getSize() const { return size; }

	static IndexBuffer* create(int size, void* data);
};