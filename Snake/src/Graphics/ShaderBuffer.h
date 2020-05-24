#pragma once

#include <GL/glew.h>

class ShaderBuffer {
private:
	unsigned int ubo = 0;
	std::size_t size = 0;
public:
	ShaderBuffer(std::size_t size, void* data) {
		this->size = size;
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void update(void* data) {
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void bind(int pos) {
		glBindBufferBase(GL_UNIFORM_BUFFER, pos, ubo);
	}
};