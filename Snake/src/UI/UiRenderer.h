#pragma once

#include "Graphics/Renderer.h"
#include "Graphics/Renderer/ShaderPipeline.h"
#include "Graphics/Renderer/ConstantBuffer.h"
#include "Graphics/Renderer/DepthState.h"
#include "Graphics/Renderer/BlendState.h"
#include "FileSystem/File.h"
#include <memory>

//struct UiData {
//	int width, height;
//	int xPos, yPos;
//};

struct ColorBufferData {
	glm::vec4 color;
	glm::vec4 bg_color;
	int has_texture = false;
	int hovered		= false;
	int selected	= false;
};

class UiRenderer
{
public:
	Renderer* renderer;
	std::unique_ptr<ShaderPipeline> ui_shader;
	std::unique_ptr<ShaderPipeline> ui_icon_shader;
	std::unique_ptr<ConstantBuffer> buffer;
	std::unique_ptr<DepthState> depthState;
	std::unique_ptr<BlendState> blend_state;
public:
	UiRenderer(Renderer* renderer): renderer(renderer)
	{
		buffer.reset(ConstantBuffer::create(0, sizeof(ColorBufferData), nullptr));

		{
			File filePS("shaders/dx/ui/pixel.hlsl");
			File fileVS("shaders/dx/ui/vertex.hlsl");
			ui_shader.reset(ShaderPipeline::create(fileVS.getConent(), filePS.getConent()));
		}
		{
			File filePS("shaders/dx/ui_icon/pixel.hlsl");
			File fileVS("shaders/dx/ui_icon/vertex.hlsl");
			ui_icon_shader.reset(ShaderPipeline::create(fileVS.getConent(), filePS.getConent()));
		}

		DepthStateDesc depthStateDesc;
		depthStateDesc.depthEnable = false;

		depthState.reset(DepthState::create(depthStateDesc));

		BlendStateDesc blendStateDesc;
		blendStateDesc.count = 1;

		blendStateDesc.blend[0].blendEnabled = true;
		blendStateDesc.blend[0].blendOp = BlendOperation::ADD;
		blendStateDesc.blend[0].srcBlend = BlendFunction::SRC_APLHA;
		blendStateDesc.blend[0].dstBlend = BlendFunction::INV_SRC_APLHA;

		blendStateDesc.blend[0].blendOpAlpha = BlendOperation::ADD;
		blendStateDesc.blend[0].srcBlendAlpha = BlendFunction::ZERO;
		blendStateDesc.blend[0].dstBlendAlpha = BlendFunction::ZERO;

		blend_state.reset(BlendState::create(blendStateDesc));
	}

	void render(UiElement* element)
	{
		depthState->bind(renderer->getContext());

		ui_shader->bind(renderer->getContext());
		render_recursive(element);

		//renderer->setDefaultDepthStencil();
	}

	void render_recursive(UiElement* el)
	{
		el->render(this);

		for (auto& child : el->childs) 
		{
			render_recursive(child);
		}
	}
};