#pragma once

#include "Graphics/Renderer/VertexBuffer.h"
#include "Graphics/Renderer/IndexBuffer.h"
#include "Graphics/Renderer/ConstantBuffer.h"
#include "Graphics/Renderer/ShaderPipeline.h"
#include "Graphics/Renderer/ShaderInputLayout.h"
#include "Graphics/Renderer/RenderTarget.h"
#include "Graphics/Renderer/Texture2D.h"
#include "Graphics/Renderer/Sampler.h"
#include "Graphics/Renderer.h"

namespace RQ {
struct RenderCommand 
{
	const char* debug_name;
	void(*execute)(Renderer*, RenderCommand*);
};

struct ClearRenderTargetCommand
{
	const char* debug_name;
	void(*execute)(Renderer*, RenderCommand*);
	RenderTarget* render_target;
};

struct DrawIndexedCommand
{
	const char* debug_name;
	void(*execute)(Renderer*, RenderCommand*);
	RenderTarget* render_target;
	int count;
};

struct BindIndexBufferCommand
{
	const char* debug_name;
	void(*execute)(Renderer*, RenderCommand*);
	IndexBuffer* index_buffer;
};

struct BindVertexBufferCommand
{
	const char* debug_name;
	void(*execute)(Renderer*, RenderCommand*);
	VertexBuffer* vertex_buffer;
};

struct BindConstantBufferCommand
{
	const char* debug_name;
	void(*execute)(Renderer*, RenderCommand*);
	ConstantBuffer* buffer;
	int slot;
};
struct UpdateBufferCommand
{
	const char* debug_name;
	void(*execute)(Renderer*, RenderCommand*);
	ConstantBuffer* buffer;
	void* data;
	int size;
};
struct BindRenderTargetCommand
{
	const char* debug_name;
	void(*execute)(Renderer*, RenderCommand*);
	RenderTarget* render_target;
};

struct BindShaderCommand
{
	const char* debug_name;
	void(*execute)(Renderer*, RenderCommand*);
	ShaderPipeline* shader;
};
struct BindShaderInputLayoutCommand
{
	const char* debug_name;
	void(*execute)(Renderer*, RenderCommand*);
	ShaderInputLayout* layout;
};

struct BindTextureCommand
{
	const char* debug_name;
	void(*execute)(Renderer*, RenderCommand*);
	Texture2D* texture;
	int slot;
};

struct BindSamplerCommand
{
	const char* debug_name;
	void(*execute)(Renderer*, RenderCommand*);
	Sampler* sampler;
	int unit;
};
struct PresentCommand
{
	const char* debug_name;
	void(*execute)(Renderer*, RenderCommand*);
};
}