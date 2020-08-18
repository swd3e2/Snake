#pragma once

#include "Graphics/Renderer/ShaderPipeline.h"
#include "Graphics/Renderer/ShaderInputLayout.h"
#include "Graphics/Renderer/VertexBuffer.h"
#include "Graphics/Renderer/ConstantBuffer.h"
#include "Graphics/Renderer/Texture2D.h"
#include "Graphics/RenderGraph/FullscreenPass.h"
#include "Graphics/RenderGraph/DefaultRenderPass.h"
#include "Graphics/RenderGraph/PhysicDebugRenderPass.h"
#include "Graphics/Renderer/Sampler.h"
#include "Graphics/Renderer/RenderTarget.h"

struct RenderSystemResource
{
	std::shared_ptr<VertexBuffer> quadVertexBuffer;
	std::shared_ptr<IndexBuffer> quadIndexBuffer;

	std::shared_ptr<FullscreenPass> light_pass;
	std::shared_ptr<FullscreenPass> ambient_occlusion_pass;
	std::shared_ptr<FullscreenPass> ambient_occlusion_blur_pass;
	std::shared_ptr<FullscreenPass> main_render_pass;
	std::shared_ptr<FullscreenPass> variance_shadow_map_blur_pass;
	std::shared_ptr<DefaultRenderPass> gbuffer_pass;
	std::shared_ptr<DefaultRenderPass> picker_pass;
	std::shared_ptr<PhysicDebugRenderPass> debug_physics_pass;
	std::shared_ptr<Pass> shadow_pass;

	std::shared_ptr<ShaderInputLayout> shader_input_layout;

	std::shared_ptr<ConstantBuffer> shader_buffer;
	std::shared_ptr<ConstantBuffer> model_shader_buffer;
	std::shared_ptr<ConstantBuffer> mesh_shader_buffer;
	std::shared_ptr<ConstantBuffer> picker_buffer;
	std::shared_ptr<ConstantBuffer> ambient_occlusion_buffer;

	MainRenderTarget* main_render_target;

	std::unordered_map<std::string, std::shared_ptr<Texture2D>> textures;
	std::unordered_map<std::string, std::shared_ptr<RenderTarget>> renderTargets;
	std::unordered_map<std::string, std::shared_ptr<ShaderPipeline>> shaders;

	std::shared_ptr<Sampler> point_sampler;
	std::shared_ptr<Sampler> point_repeat_sampler;
	std::shared_ptr<Sampler> linear_clamp_sampler;
};