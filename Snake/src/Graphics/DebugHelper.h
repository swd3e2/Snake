#pragma once

#include "Renderer/Texture2D.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/VertexBuffer.h"
#include <memory>
#include "Renderer.h"
#include <vector>
#include "vertex.h"

class DebugHelper {
private:
	VertexBuffer* vbuffer = nullptr;
	IndexBuffer*  ibuffer = nullptr;
public:
	DebugHelper()
	{
		std::vector<vertex> vertices;
		vertices.push_back(vertex(0.0f, 10.0f, 0.0f, 0.0f, 0.0f));
		vertices.push_back(vertex(0.0f, -10.0f, 0.0f, 0.0f, 1.0f));

		vbuffer = VertexBuffer::create(vertices.size(), sizeof(vertex), vertices.data(), true);
	}

	void renderTexture(int x, int y, int width, int height, Texture2D* texture)
	{
		Renderer* renderer = Renderer::instance();
		RenderContext* context = renderer->getContext();

		texture->bindToUnit(0, context);
	}

	void renderFont()
	{

	}
};