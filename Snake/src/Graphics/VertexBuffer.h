#pragma once

#include <GL/glew.h>

class VertexBuffer {
private:
	unsigned int vao = 0;
	unsigned int vbo = 0;
	int stride = 0;
public:
	VertexBuffer(int size, int stride, void* data) :
		stride(stride) 
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glCreateBuffers(1, &vbo);
		glNamedBufferStorage(vbo, size * stride, data, 0);

		glBindVertexArray(0);
	}

	void bind() {
		glBindVertexBuffer(0, vbo, 0, stride);
	}
};