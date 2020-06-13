#pragma once

#include <string>
#include <glad/glad.h>

enum InputDataType
{
	Float,
	Float2,
	Float3,
	Float4,
	Int,
	Int2,
	Int3,
	Int4,
	Bool
};

class ShaderInputLayout {
private:
	unsigned int stride = 0;
	unsigned int vao = 0;
	int size = 0;
public:
	struct InputLayoutElement
	{
		InputDataType type;
		std::string name;
		int offset;

		InputLayoutElement(InputDataType type, const std::string& name) :
			type(type), name(name)
		{}

		const int getSize() const
		{
			switch (type)
			{
			case Float:		return 4;
			case Float2:	return 4 * 2;
			case Float3:	return 4 * 3;
			case Float4:	return 4 * 4;
			case Int:		return 4;
			case Int2:		return 4 * 2;
			case Int3:		return 4 * 3;
			case Int4:		return 4 * 4;
			case Bool:		return 4;
			}
			return 0;
		}

		const int getNumberElements() const
		{
			switch (type)
			{
			case Float:		return 1;
			case Float2:	return 2;
			case Float3:	return 3;
			case Float4:	return 4;
			case Int:		return 1;
			case Int2:		return 2;
			case Int3:		return 3;
			case Int4:		return 4;
			case Bool:		return 1;
			}
			return 0;
		}
	};
public:
	ShaderInputLayout() {
		glCreateVertexArrays(1, &vao);
	}

	void add(const InputLayoutElement& element)
	{
		GLuint type = getInputLayoutFormat(element.type);

		glEnableVertexArrayAttrib(vao, size);
		if (type == GL_FLOAT) {
			glVertexArrayAttribFormat(vao, size, element.getNumberElements(), type, GL_FALSE, stride);
		} else if (type == GL_INT || type == GL_BOOL) {
			glVertexArrayAttribIFormat(vao, size, element.getNumberElements(), type, stride);
		}
		glVertexArrayAttribBinding(vao, size, 0);

		stride += element.getSize();
		size++;
	}

	void bind()
	{
		glBindVertexArray(vao);
	}
private:
	GLuint getInputLayoutFormat(const InputDataType type)
	{
		switch (type)
		{
		case Float:
		case Float2:
		case Float3:
		case Float4:
			return GL_FLOAT;
		case Int:
		case Int2:
		case Int3:
		case Int4:
			return GL_INT;
		case Bool:
			return GL_BOOL;
		}
		return GL_FLOAT;
	}
};