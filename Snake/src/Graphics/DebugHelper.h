#pragma once

#include "Renderer/Texture2D.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/ShaderPipeline.h"
#include <memory>
#include "Renderer.h"
#include <vector>
#include "vertex.h"
#include <string>
#include "FontRenderer.h"

class DebugHelper {
private:
	FontRenderer fontRenderer;
	VertexBuffer* vbuffer = nullptr;
	ShaderPipeline* fontShader = nullptr;
	ShaderPipeline* textureShader = nullptr;
public:
	DebugHelper()
	{
		vertex vertices[] = {
			vertex(1.0f,  1.0f, 0.0f, 1.0f, 0.0f),
			vertex(1.0f, -1.0f, 0.0f, 1.0f, 1.0f),
			vertex(-1.0f,  1.0f, 0.0f, 0.0f, 0.0f),

			vertex(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f),
			vertex(-1.0f,  1.0f, 0.0f, 0.0f, 0.0f),
			vertex(1.0f, -1.0f, 0.0f, 1.0f, 1.0f),
		};
		vbuffer = VertexBuffer::create(6, sizeof(vertex), vertices, true);

		{
			File filePS("shaders/dx/debug_font/pixel.hlsl");
			File fileVS("shaders/dx/debug_font/vertex.hlsl");
			fontShader = ShaderPipeline::create(fileVS.getConent(), filePS.getConent());
		}
		{
			File filePS("shaders/dx/debug_texture/pixel.hlsl");
			File fileVS("shaders/dx/debug_texture/vertex.hlsl");
			textureShader = ShaderPipeline::create(fileVS.getConent(), filePS.getConent());
		}
	}

	void renderTexture(float startX, float startY, float width, float height, Texture2D* texture)
	{
		Renderer* renderer = Renderer::instance();
		RenderContext* context = renderer->getContext();
		ShaderPipeline* prevShader = context->getCurrentShader();

		startX = startX * 2.0 - 1.0f;
		startY = startY * 2.0 - 1.0f;
		width = width * 2.0f;
		height = height * 2.0f;

		vertex vertices[] = {
			vertex(startX + width,  startY + height,	0.0f, 1.0f, 0.0f),
			vertex(startX + width,  startY,				0.0f, 1.0f, 1.0f),
			vertex(startX,			startY + height,	0.0f, 0.0f, 0.0f),

			vertex(startX,			startY,				0.0f, 0.0f, 1.0f),
			vertex(startX,			startY + height,	0.0f, 0.0f, 0.0f),
			vertex(startX + width,	startY,				0.0f, 1.0f, 1.0f),
		};

		textureShader->bind(context);
		vbuffer->update(vertices, 6, sizeof(vertex));
		vbuffer->bind(context);
		texture->bindToUnit(0, context);

		renderer->draw(6);

		prevShader->bind(context);
	}

	void renderFont(const std::string& text, float x, float y, float scale)
	{
		Renderer* renderer = Renderer::instance();
		RenderContext* context = renderer->getContext();
		ShaderPipeline* prevShader = context->getCurrentShader();

		fontShader->bind(context);
		
		x = x * 2.0f - 1.0f;
		y = y * 2.0f - 1.0f;

		fontRenderer.renderFont(text, x, y, scale);
	}
};