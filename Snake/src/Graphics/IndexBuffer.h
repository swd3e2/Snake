#pragma once

#include <GL/glew.h>

class IndexBuffer {
private:
	unsigned int vbo = 0;
	int size = 0;
public:
	IndexBuffer(int size, void* data) :
		size(size)
	{
		glCreateBuffers(1, &vbo);
		glNamedBufferStorage(vbo, size * 4, data, 0);
	}

	const int getSize() const { return size; }

	void bind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
	}
};