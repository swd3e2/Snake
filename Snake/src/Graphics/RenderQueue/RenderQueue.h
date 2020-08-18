#pragma once

#include "RenderCommands.h"
#include "Memory/LinearAllocator.h"
#include <queue>
#include <functional>

class RenderQueue {
private:
	LinearAllocator* allocator;
public:
	std::array<RQ::RenderCommand*, 100000> commands;
	size_t commands_size = 0;
public:
	RenderQueue()
	{
		allocator = new LinearAllocator(10 * 1024 * 1024);
	}

	void drawIndexed(int count)
	{
		using namespace RQ;
		DrawIndexedCommand* command = (DrawIndexedCommand*)allocator->allocate(sizeof(DrawIndexedCommand));
		command->debug_name = "DrawIndexed";
		command->count = count;

		command->execute = [](Renderer* renderer, RenderCommand* command) {
			DrawIndexedCommand* com = (DrawIndexedCommand*)command;
			renderer->drawIndexed(com->count);
		};

		addCommand((RenderCommand*)command);
	}

	void bindIndexBuffer(IndexBuffer* index_buffer)
	{
		using namespace RQ;

		BindIndexBufferCommand* command = (BindIndexBufferCommand*)allocator->allocate(sizeof(BindIndexBufferCommand));
		command->debug_name = "BindIndexBuffer";
		command->index_buffer = index_buffer;

		command->execute = [](Renderer* renderer, RenderCommand* command) {
			BindIndexBufferCommand* com = (BindIndexBufferCommand*)command;
			com->index_buffer->bind(renderer->getContext());
		};

		addCommand((RenderCommand*)command);
		/*BindIndexBufferCommand* command = new (allocator->allocate(sizeof(BindIndexBufferCommand))) BindIndexBufferCommand();
		command->index_buffer = index_buffer;
		addCommand(command);*/
	}

	void bindVertexBuffer(VertexBuffer* vertex_buffer)
	{
		using namespace RQ;

		BindVertexBufferCommand* command = (BindVertexBufferCommand*)allocator->allocate(sizeof(BindVertexBufferCommand));
		command->debug_name = "BindVertexBuffer";
		command->vertex_buffer = vertex_buffer;

		command->execute = [](Renderer* renderer, RenderCommand* command) {
			BindVertexBufferCommand* com = (BindVertexBufferCommand*)command;
			com->vertex_buffer->bind(renderer->getContext());
		};

		addCommand((RenderCommand*)command);

		/*BindVertexBufferCommand* command = new (allocator->allocate(sizeof(BindVertexBufferCommand))) BindVertexBufferCommand();
		command->vertex_buffer = vertex_buffer;
		addCommand(command);*/
	}

	void updateConstantBuffer(ConstantBuffer* buffer, void* data, int size)
	{
		using namespace RQ;

		void* data_copy = allocator->allocate(size);
		memcpy(data_copy, data, size);

		UpdateBufferCommand* command = (UpdateBufferCommand*)allocator->allocate(sizeof(UpdateBufferCommand));
		command->debug_name = "UpdateBuffer";
		command->buffer = buffer;
		command->data = data_copy;
		command->size = size;

		command->execute = [](Renderer* renderer, RenderCommand* command) {
			UpdateBufferCommand* com = (UpdateBufferCommand*)command;
			com->buffer->update(com->data);
		};

		addCommand((RenderCommand*)command);

		/*UpdateBufferCommand* command = new (allocator->allocate(sizeof(UpdateBufferCommand))) UpdateBufferCommand();
		command->buffer = buffer;
		command->data = data_copy;
		command->size = size;

		addCommand(command);*/
	}

	void bindShader(ShaderPipeline* shader)
	{
		using namespace RQ;

		BindShaderCommand* command = (BindShaderCommand*)allocator->allocate(sizeof(BindShaderCommand));
		command->debug_name = "BindShader";
		command->shader = shader;

		command->execute = [](Renderer* renderer, RenderCommand* command) {
			BindShaderCommand* com = (BindShaderCommand*)command;
			com->shader->bind(renderer->getContext());
		};

		addCommand((RenderCommand*)command);

		/*BindShaderCommand* command = new (allocator->allocate(sizeof(BindShaderCommand))) BindShaderCommand();
		command->shader = shader;
		addCommand(command);*/
	}

	void bindConstantBuffer(ConstantBuffer* buffer, int slot)
	{
		using namespace RQ;

		BindConstantBufferCommand* command = (BindConstantBufferCommand*)allocator->allocate(sizeof(BindConstantBufferCommand));
		command->debug_name = "BindConstantBuffer";
		command->buffer = buffer;
		command->slot = slot;

		command->execute = [](Renderer* renderer, RenderCommand* command) {
			BindConstantBufferCommand* com = (BindConstantBufferCommand*)command;
			com->buffer->bind(renderer->getContext());
		};

		addCommand((RenderCommand*)command);
		/*BindConstantBufferCommand* command = new (allocator->allocate(sizeof(BindConstantBufferCommand))) BindConstantBufferCommand();
		command->buffer = buffer;
		command->slot = slot;
		addCommand(command);*/
	}

	void bindShaderInputLayout(ShaderInputLayout* layout) 
	{
		using namespace RQ;

		BindShaderInputLayoutCommand* command = (BindShaderInputLayoutCommand*)allocator->allocate(sizeof(BindShaderInputLayoutCommand));
		command->debug_name = "BindShaderInputLayout";
		command->layout = layout;

		command->execute = [](Renderer* renderer, RenderCommand* command) {
			BindShaderInputLayoutCommand* com = (BindShaderInputLayoutCommand*)command;
			com->layout->bind(renderer->getContext());
		};

		addCommand((RenderCommand*)command);
		/*BindShaderInputLayoutCommand* command = new (allocator->allocate(sizeof(BindShaderInputLayoutCommand))) BindShaderInputLayoutCommand();
		command->layout = layout;
		addCommand(command);*/
	}

	void bindRenderTarget(RenderTarget* render_target)
	{
		using namespace RQ;

		BindRenderTargetCommand* command = (BindRenderTargetCommand*)allocator->allocate(sizeof(BindRenderTargetCommand));
		command->debug_name = "BindRenderTarget";
		command->render_target = render_target;

		command->execute = [](Renderer* renderer, RenderCommand* command) {
			BindRenderTargetCommand* com = (BindRenderTargetCommand*)command;
			com->render_target->bind(renderer->getContext());
		};

		addCommand((RenderCommand*)command);
		/*BindRenderTargetCommand* command = new (allocator->allocate(sizeof(BindRenderTargetCommand))) BindRenderTargetCommand();
		command->render_target = render_target;
		addCommand(command);*/
	}

	void bindSampler(Sampler* sampler, int unit)
	{
		using namespace RQ;

		BindSamplerCommand* command = (BindSamplerCommand*)allocator->allocate(sizeof(BindSamplerCommand));
		command->debug_name = "BindSampler";
		command->sampler = sampler;

		command->execute = [](Renderer* renderer, RenderCommand* command) {
			BindSamplerCommand* com = (BindSamplerCommand*)command;
			com->sampler->bind(renderer->getContext());
		};

		addCommand((RenderCommand*)command);

		/*BindSamplerCommand* command = new (allocator->allocate(sizeof(BindSamplerCommand))) BindSamplerCommand();
		command->sampler = sampler;
		command->unit = unit;
		addCommand(command);*/
	}

	void bindTexture(Texture2D* texture, int slot)
	{
		using namespace RQ;

		BindTextureCommand* command = (BindTextureCommand*)allocator->allocate(sizeof(BindTextureCommand));
		command->debug_name = "BindTexture";
		command->texture = texture;
		command->slot = slot;

		command->execute = [](Renderer* renderer, RenderCommand* command) {
			BindTextureCommand* com = (BindTextureCommand*)command;
			com->texture->bindToUnit(com->slot, renderer->getContext());
		};

		addCommand((RenderCommand*)command);

		/*BindTextureCommand* command = new (allocator->allocate(sizeof(BindTextureCommand))) BindTextureCommand();
		command->texture = texture;
		command->slot = slot;
		addCommand(command);*/
	}

	void clearRenderTarget(RenderTarget* render_target)
	{
		using namespace RQ;

#if 0
		Command* command = (Command*)allocator->allocate(sizeof(Command));
		RenderTarget** render_target_mem = (RenderTarget**)allocator->allocate(sizeof(RenderTarget*));
		*render_target_mem = render_target;

		command->debug_name = "Present";
		command->execute = [](Renderer* renderer, RenderCommand* command) {
			RenderTarget* render_target = *(RenderTarget**)((uintptr_t)command + sizeof(Command));
			render_target->clear(renderer->getContext());
		};
		addCommand(command);
#endif
		ClearRenderTargetCommand* command = (ClearRenderTargetCommand*)allocator->allocate(sizeof(ClearRenderTargetCommand));
		command->debug_name = "Present";
		command->render_target = render_target;
		command->execute = [](Renderer* renderer, RenderCommand* command) {
			ClearRenderTargetCommand* com = (ClearRenderTargetCommand*)command;
			com->render_target->clear(renderer->getContext());
		};
		addCommand((RenderCommand*)command);
	}

	void present()
	{
		using namespace RQ;

		PresentCommand* command = (PresentCommand*)allocator->allocate(sizeof(PresentCommand));
		command->debug_name = "Present";
		command->execute = [](Renderer* renderer, RenderCommand* command) {
			renderer->swapBuffers();
		};
		addCommand((RenderCommand*)command);
	}

	void clear()
	{
		commands_size = 0;
		allocator->clear();
	}
private:
	void addCommand(RQ::RenderCommand* command)
	{
		commands[commands_size++] = command;
	}
};