#pragma once

#include "RenderSystemResource.h"
#include "ApplicationSettings.h"
#include "Graphics/Renderer.h"
#include <string>

struct ModelData {
	glm::mat4 toWorld;
	glm::mat4 inverseToWorld;
	int selected = 0;
};

struct MeshData {
	glm::mat4 transform;
	glm::mat4 normalTransform;

	struct {
		glm::vec3 ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
		float shininess = 64.0f;
		glm::vec3 diffuseColor = glm::vec3(1.0f);
		float dummy;
		glm::vec3 specularColor = glm::vec3(1.0f);
		int hasDiffuseMap = 0;
		int hasNormalMap = 0;
	} materialData;
};

class RenderSystemResourceManager
{
public:
	RenderSystemResource* resource = nullptr;
public:
	~RenderSystemResourceManager()
	{
		deinitialize();
	}

	RenderSystemResource* initialize(Renderer* renderer, ApplicationSettings* settings)
	{
		assert(!resource && "Render system resources already initialized");

		resource = new RenderSystemResource();

		initBuffers(resource);
		initAbmientOcclusionData(resource);
		initShaderInputLayout(resource);
		initSamplers(resource);
		initRenderTargetTextures(settings, resource);
		initRenderTargets(resource);
		initShaders(resource);
		
		std::vector<vertex> vertexData;
		vertexData.push_back(vertex(1.0f, 1.0f, 0.1f, 1.0f, 0.0f));
		vertexData.push_back(vertex(1.0f, -1.0f, 0.1f, 1.0f, 1.0f));
		vertexData.push_back(vertex(-1.0f, 1.0f, 0.1f, 0.0f, 0.0f));
		vertexData.push_back(vertex(-1.0f, -1.0f, 0.1f, 0.0f, 1.0f));
		resource->quadVertexBuffer.reset(VertexBuffer::create(vertexData.size(), sizeof(vertex), vertexData.data()));

		std::vector<unsigned int> indexData;
		indexData.push_back(0); indexData.push_back(2); indexData.push_back(1);
		indexData.push_back(2); indexData.push_back(3); indexData.push_back(1);

		resource->quadIndexBuffer.reset(IndexBuffer::create(indexData.size(), indexData.data()));
		resource->main_render_target = renderer->getMainRenderTarget();
		
		return resource;
	}
private:
	void initBuffers(RenderSystemResource* resource)
	{
		resource->shader_buffer.reset(ConstantBuffer::create(0, sizeof(WVP), nullptr));
		resource->model_shader_buffer.reset(ConstantBuffer::create(1, sizeof(ModelData), nullptr));
		resource->mesh_shader_buffer.reset(ConstantBuffer::create(2, sizeof(MeshData), nullptr));
		resource->picker_buffer.reset(ConstantBuffer::create(3, sizeof(glm::vec4), nullptr));
		resource->ambient_occlusion_buffer.reset(ConstantBuffer::create(2, sizeof(glm::vec4) * 64, nullptr));
	}

	void initAbmientOcclusionData(RenderSystemResource* resource)
	{
		std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
		std::default_random_engine generator;
		std::vector<glm::vec4> ssaoKernel;
		for (unsigned int i = 0; i < 64; ++i)
		{
			glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
			sample = glm::normalize(sample);
			sample *= randomFloats(generator);
			float scale = float(i) / 64.0;

			// scale samples s.t. they're more aligned to center of kernel
			scale = lerp(0.1f, 1.0f, scale * scale);
			sample *= scale;
			ssaoKernel.push_back(glm::vec4(sample, 0));
		}
		resource->ambient_occlusion_buffer->update(ssaoKernel.data());

		std::vector<glm::vec3> ssaoNoise;
		for (unsigned int i = 0; i < 16; i++)
		{
			glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
			ssaoNoise.push_back(noise);
		}

		resource->textures["noise"] = std::shared_ptr<Texture2D>(Texture2D::create(4, 4, 0, ssaoNoise.data(), TextureFormat::RGB32F, TextureFlags::TF_ShaderResource));
	}

	void initShaderInputLayout(RenderSystemResource* resource)
	{
		ShaderInputLayoutDesc desc;
		desc.elements.push_back({ InputDataType::Float3, "POSITION" });
		desc.elements.push_back({ InputDataType::Float2, "TEXCOORD" });
		desc.elements.push_back({ InputDataType::Float3, "NORMAL" });
		desc.elements.push_back({ InputDataType::Float3, "TANGENT" });
		desc.elements.push_back({ InputDataType::Float3, "BITANGENT" });
		desc.elements.push_back({ InputDataType::Int4,   "BONE" });
		desc.elements.push_back({ InputDataType::Float4, "WEIGHT" });

		resource->shader_input_layout.reset(ShaderInputLayout::create(desc));
	}

	void initSamplers(RenderSystemResource* resource)
	{
		// Samplers
		{
			SamplerDesc desc;
			desc.minFilterModel = FilterMode::POINT;
			desc.magFilterMode = FilterMode::POINT;
			desc.mipFilterMode = FilterMode::POINT;
			desc.addressingMode = AddressingMode::WRAP;
			resource->point_repeat_sampler.reset(Sampler::create(desc));
			//resource->pointRepeatSampler->setSamplerUnit(0);
		}
		{
			SamplerDesc desc;
			desc.minFilterModel = FilterMode::POINT;
			desc.magFilterMode = FilterMode::POINT;
			desc.mipFilterMode = FilterMode::POINT;
			desc.addressingMode = AddressingMode::CLAMP;
			resource->point_sampler.reset(Sampler::create(desc));
			resource->point_sampler->setSamplerUnit(1);
			//resource->pointSampler->bind(context);
		}
		{
			SamplerDesc desc;
			desc.minFilterModel = FilterMode::LINEAR;
			desc.magFilterMode = FilterMode::LINEAR;
			desc.mipFilterMode = FilterMode::LINEAR;
			desc.addressingMode = AddressingMode::CLAMP;
			resource->linear_clamp_sampler.reset(Sampler::create(desc));
			resource->linear_clamp_sampler->setSamplerUnit(2);
			//resource->linearClampSampler->bind(context);
		}
	}

	void initRenderTargetTextures(ApplicationSettings* settings, RenderSystemResource* resource)
	{
		int width = settings->getWindowWidth();
		int height = settings->getWindowHeight();		
		
		int shadow_width = settings->getShadowMapWidth();
		int shadow_height = settings->getShadowMapHeight();

		resource->textures["positions"]					= std::shared_ptr<Texture2D>(Texture2D::create(width, height, 0, nullptr, TextureFormat::RGBA32F, TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
		resource->textures["normals"]					= std::shared_ptr<Texture2D>(Texture2D::create(width, height, 1, nullptr, TextureFormat::RGBA32F, TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
		resource->textures["diffuse"]					= std::shared_ptr<Texture2D>(Texture2D::create(width, height, 2, nullptr, TextureFormat::RGBA32F, TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
		resource->textures["view_space_pos"]			= std::shared_ptr<Texture2D>(Texture2D::create(width, height, 2, nullptr, TextureFormat::RGBA32F, TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
		resource->textures["view_space_normal"]			= std::shared_ptr<Texture2D>(Texture2D::create(width, height, 2, nullptr, TextureFormat::RGBA16F, TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
		resource->textures["rtt"]						= std::shared_ptr<Texture2D>(Texture2D::create(width, height, 0, nullptr, TextureFormat::RGBA16F, TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));

		resource->textures["g_buffer_depth"]			= std::shared_ptr<Texture2D>(Texture2D::create(width, height, 3, nullptr, TextureFormat::D24S8, TextureFlags::TF_DepthBuffer | TextureFlags::TF_ShaderResource));
		resource->textures["picker"]					= std::shared_ptr<Texture2D>(Texture2D::create(width, height, 0, nullptr, TextureFormat::RGBA8, TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
		resource->textures["pickerDepth"]				= std::shared_ptr<Texture2D>(Texture2D::create(width, height, 0, nullptr, TextureFormat::D24S8, TextureFlags::TF_DepthBuffer | TextureFlags::TF_ShaderResource));
		resource->textures["ambient_occlusion_color"]	= std::shared_ptr<Texture2D>(Texture2D::create(width, height, 0, nullptr, TextureFormat::RGBA8, TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));

		resource->textures["shadowColorTexture"]		= std::shared_ptr<Texture2D>(Texture2D::create(shadow_width, shadow_width, 5, nullptr, TextureFormat::RGBA32F, TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
		resource->textures["bluredShadowDepthTexture"]	= std::shared_ptr<Texture2D>(Texture2D::create(shadow_width, shadow_width, 5, nullptr, TextureFormat::RGBA32F, TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource));
		resource->textures["shadowDepthTexture"]		= std::shared_ptr<Texture2D>(Texture2D::create(shadow_width, shadow_width, 5, nullptr, TextureFormat::D24S8, TextureFlags::TF_DepthBuffer | TextureFlags::TF_ShaderResource));
	}

	void initRenderTargets(RenderSystemResource* resource)
	{
		{
			std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
			renderTarget->setColorTexture(resource->textures["positions"], 0);
			renderTarget->setColorTexture(resource->textures["normals"], 1);
			renderTarget->setColorTexture(resource->textures["diffuse"], 2);
			renderTarget->setColorTexture(resource->textures["view_space_pos"], 3);
			renderTarget->setColorTexture(resource->textures["view_space_normal"], 4);
			renderTarget->setDepthTexture(resource->textures["g_buffer_depth"]);
			resource->renderTargets["gBuffer"] = renderTarget;
		}
		{
			std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
			renderTarget->setColorTexture(resource->textures["shadowColorTexture"], 0);
			renderTarget->setDepthTexture(resource->textures["shadowDepthTexture"]);
			resource->renderTargets["shadow"] = renderTarget;
		}
		{
			std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
			renderTarget->setColorTexture(resource->textures["rtt"], 0);
			resource->renderTargets["light"] = renderTarget;
		}
		{
			std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
			renderTarget->setColorTexture(resource->textures["bluredShadowDepthTexture"], 0);
			resource->renderTargets["blur"] = renderTarget;
		}
		// Ambient occlusion
		{
			std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
			renderTarget->setColorTexture(resource->textures["ambient_occlusion_color"], 0);
			resource->renderTargets["ambient_occlusion"] = renderTarget;
		}
		// Selection render target
		{
			std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
			renderTarget->setColorTexture(resource->textures["picker"], 0);
			renderTarget->setDepthTexture(resource->textures["pickerDepth"], 0);
			resource->renderTargets["picker"] = renderTarget;
		}
	}

	void initShaders(RenderSystemResource* resource)
	{
		createShaderFromFolder(resource, "shaders/dx/default", "default");
		createShaderFromFolder(resource, "shaders/dx/gbuffer", "gbuffer");
		createShaderFromFolder(resource, "shaders/dx/shadow", "shadow");
		createShaderFromFolder(resource, "shaders/dx/light", "light");
		createShaderFromFolder(resource, "shaders/dx/blur", "blur");
		createShaderFromFolder(resource, "shaders/dx/physdebug", "physdebug");
		createShaderFromFolder(resource, "shaders/dx/gui", "gui");
		createShaderFromFolder(resource, "shaders/dx/picker", "picker");
		createShaderFromFolder(resource, "shaders/dx/ambient_occlusion", "ambient_occlusion");
	}

	void createShaderFromFolder(RenderSystemResource* resource, const std::string& folder, const std::string& name)
	{
		std::string vertexShaderFilename;
		std::string pixelShaderFilename;
		std::string geometryShaderFilename;

		if (Renderer::getType() == RendererType::OpenGL) {
			vertexShaderFilename = "/pixel.glsl";
			pixelShaderFilename = "/vertex.glsl";
			geometryShaderFilename = "/geometry.glsl";
		}
		else {
			vertexShaderFilename = "/vertex.hlsl";
			pixelShaderFilename = "/pixel.hlsl";
			geometryShaderFilename = "/geometry.hlsl";
		}

		File filePS(folder + pixelShaderFilename);
		File fileVS(folder + vertexShaderFilename);
		File fileGS(folder + geometryShaderFilename);

		if (!filePS.exists() || !fileVS.exists()) {
			std::cout << "Could not create shader from path " << folder << std::endl;
			return;
		}

		resource->shaders[name] = std::shared_ptr<ShaderPipeline>(
			ShaderPipeline::create(fileVS.getConent(), filePS.getConent())
		);
	}

	void deinitialize()
	{
		//...
	}
};