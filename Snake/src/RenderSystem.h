#pragma once

#include "Input/InputManager.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Graphics/Renderer/ShaderPipeline.h"
#include "Graphics/Renderer/ShaderInputLayout.h"
#include "Graphics/Renderer/VertexBuffer.h"
#include "Graphics/Renderer/ConstantBuffer.h"
#include "Graphics/Renderer/Texture2D.h"
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include <entt/entt.hpp>
#include "Components.h"
#include "Graphics/vertex.h"
#include "Camera.h"
#include "Model/Model.h"
#include <queue>
#include "Graphics/Platform/OpenGL/OpenGLRenderer.h"
#include "FileSystem/File.h"
#include "Graphics/RenderGraph/RenderGraph.h"
#include "Graphics/Renderer/RenderTarget.h"
#include "Import/RawTexture.h"
#include "Graphics/RenderGraph/FullscreenPass.h"
#include "Graphics/Renderer/Sampler.h"
#include "Physics/PhysicsSystem.h"

class RenderSystem {
private:
    struct mvp {
        glm::mat4 projection;
        glm::mat4 shadowProjection;
        glm::vec4 shadowCameraPos;
        glm::vec4 viewPos;
    };
    struct ModelData {
        glm::mat4 toWorld;
        glm::mat4 inverseToWorld;
    };
	struct MaterialData {
		glm::vec3 ambientColor = glm::vec3(1.0f, 0.0f, 1.0f);
        float shininess = 64.0f;
		glm::vec3 diffuseColor = glm::vec3(1.0f);
        float dummy;
		glm::vec3 specularColor = glm::vec3(1.0f);
	};
    struct MeshData {
        glm::mat4 transform;
        glm::mat4 normalTransform;
        MaterialData materialData;
    };
public:
    mvp projectionData;
    ModelData modelData;
    MeshData meshData;
    Renderer* renderer;
    RenderContext* context;
    std::unique_ptr<RenderGraph> renderGraph;

    std::shared_ptr<ShaderPipeline> shaderPipeline;
    std::shared_ptr<ShaderInputLayout> shaderInputLayout;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<ConstantBuffer> shaderBuffer;
    std::shared_ptr<ConstantBuffer> modelShaderBuffer;
    std::shared_ptr<ConstantBuffer> meshShaderBuffer;
    std::shared_ptr<ConstantBuffer> materialShaderBuffer;
    
    std::shared_ptr<MainRenderTarget> mainRenderTarget;
    entt::registry* registry;
    
    Camera camera;
    Camera lightCamera;

    std::shared_ptr<FullscreenPass> lightGBufferPass;
	std::shared_ptr<FullscreenPass> testRenderPass;
	std::shared_ptr<FullscreenPass> blurPass;
    std::shared_ptr<Pass> gbufferPass;
    std::shared_ptr<Pass> shadowPass;
    int mainCamera = 0;

    std::unordered_map<std::string, std::shared_ptr<Texture2D>> textures;
    std::unordered_map<std::string, std::shared_ptr<RenderTarget>> renderTargets;
    std::unordered_map<std::string, std::shared_ptr<ConstantBuffer>> constantBuffers;
    std::unordered_map<std::string, std::shared_ptr<ShaderPipeline>> shaders;

	std::shared_ptr<RenderTarget> rt;
	std::shared_ptr<RenderTarget> rt2;
	std::shared_ptr<RenderTarget> shadowRt;
	std::shared_ptr<Sampler> pointSampler;

    GLuint voxelTexture;
    PhysicsSystem* physicsSystem;
    bool useDebugCamera = true;
public:
	RenderSystem(entt::registry* registry, PhysicsSystem* physics) {
        this->registry = registry;
        this->physicsSystem = physics;
        renderer = Renderer::instance();
        context = renderer->getContext();

        File fileVS("defaultVS.glsl");
        File filePS("defaultPS.glsl");

        shaderPipeline.reset(ShaderPipeline::create(fileVS.getConent(), filePS.getConent()));

		{
			ShaderInputLayoutDesc desc;
			desc.elements.push_back({ InputDataType::Float3, "pos" });
			desc.elements.push_back({ InputDataType::Float2, "texCoords" });
			desc.elements.push_back({ InputDataType::Float3, "normal" });
			desc.elements.push_back({ InputDataType::Float3, "tangent" });
			desc.elements.push_back({ InputDataType::Float3, "bitangent" });
			desc.elements.push_back({ InputDataType::Int4,   "bone" });
			desc.elements.push_back({ InputDataType::Float4, "weight" });

			shaderInputLayout.reset(ShaderInputLayout::create(desc));
		}
        /*{
			shaderInputLayout = std::make_shared<ShaderInputLayout>();

			shaderInputLayout->add({ InputDataType::Float3, "pos" });
			shaderInputLayout->add({ InputDataType::Float2, "texCoords" });
			shaderInputLayout->add({ InputDataType::Float3, "normal" });
			shaderInputLayout->add({ InputDataType::Float3, "tangent" });
			shaderInputLayout->add({ InputDataType::Float3, "bitangent" });
			shaderInputLayout->add({ InputDataType::Int4,   "bone" });
			shaderInputLayout->add({ InputDataType::Float4, "weight" });
        }*/

        shaderBuffer.reset(ConstantBuffer::create(0, sizeof(projectionData), nullptr));
        modelShaderBuffer.reset(ConstantBuffer::create(1, sizeof(ModelData), nullptr));
        meshShaderBuffer.reset(ConstantBuffer::create(2, sizeof(MeshData), nullptr));

        std::shared_ptr<RawTexture> texture = std::make_shared<RawTexture>("texture_01.png");
        texture->import();
        textures["tex"] = std::shared_ptr<Texture2D>(Texture2D::create(texture->getWidth(), texture->getHeight(), 0, texture->getData(), texture->getChannels() == 4 ? TextureFormat::RGBA8 : TextureFormat::RGB8));


        camera.setProjectionMatrix(90.0f, 1920.0f / 1080.0f, 0.1f, 200.f);
        lightCamera.setOrthographicMatrix(-40, 40, -40, 40, -40, 40);
        //lightCamera.m_Rotation = glm::vec4(0.0f, 0.5f, 0.0f, 1.0f);
        lightCamera.m_Position = glm::vec4(7.0f, 4.0f, -2.0f, 1.0f);

        mainRenderTarget.reset(Renderer::getMainRenderTarget());

        renderGraph = std::make_unique<RenderGraph>(renderer);

        {
			SamplerDesc desc;
			desc.minFilterModel = FilterMode::LINEAR;
			desc.magFilterMode = FilterMode::LINEAR;
			desc.mipFilterMode = FilterMode::NONE;
			desc.addressingMode = AddressingMode::CLAMP;
			pointSampler.reset(Sampler::create(desc));
			pointSampler->setTextureUnit(5);
			pointSampler->bind(context);
			pointSampler->setTextureUnit(0);
			pointSampler->bind(context);
        }
        // Textures
        {
			textures["positions"] = std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 0, nullptr, TextureFormat::RGBA32F));
			textures["normals"] = std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 1, nullptr, TextureFormat::RGBA32F));
			textures["diffuse"] = std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 2, nullptr, TextureFormat::RGBA32F));
			textures["rtt"] = std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 0, nullptr, TextureFormat::RGBA32F));
			textures["shadowColorTexture"] = std::shared_ptr<Texture2D>(Texture2D::create(1024, 1024, 5, nullptr, TextureFormat::RG32F));
			textures["bluredShadowDepthTexture"] = std::shared_ptr<Texture2D>(Texture2D::create(1024, 1024, 5, nullptr, TextureFormat::RG32F));
			textures["shadowDepthTexture"] = std::shared_ptr<Texture2D>(Texture2D::create(1024, 1024, 5, nullptr, TextureFormat::D32));
			textures["renderTargetDepthTexture"] = std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 3, nullptr, TextureFormat::D32));
        }
        // Shaders
        {
    		createShaderFromFolder("shaders/gbuffer", "gbuffer");
			createShaderFromFolder("shaders/shadow", "shadow");
			createShaderFromFolder("shaders/light", "light");
			createShaderFromFolder("shaders/blur", "blur");
			createShaderFromFolder("shaders/physdebug", "physdebug");
        }
        // Render targets
        {
            {
				std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
				renderTarget->setColorTexture(textures["positions"], 0);
				renderTarget->setColorTexture(textures["normals"], 1);
				renderTarget->setColorTexture(textures["diffuse"], 2);
				renderTarget->setDepthTexture(textures["renderTargetDepthTexture"]);
				renderTargets["gBuffer"] = renderTarget;
			}
			{
				std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
				renderTarget->setColorTexture(textures["shadowColorTexture"], 0);
				renderTarget->setDepthTexture(textures["shadowDepthTexture"]);
				renderTargets["shadow"] = renderTarget;
			}
			{
				std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
				renderTarget->setColorTexture(textures["rtt"], 0);
				renderTargets["light"] = renderTarget;
			}
			{
				std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
				renderTarget->setColorTexture(textures["bluredShadowDepthTexture"], 0);
				renderTargets["blur"] = renderTarget;
			}
        }
        // Render passess
        {
            gbufferPass.reset(new Pass("GBufferPass"));
            gbufferPass->setRenderTarget(renderTargets["gBuffer"]);
            gbufferPass->setShader(shaders["gbuffer"]);
            gbufferPass->setTexture(0, textures["tex"]);
            renderGraph->addPass(gbufferPass);
		}
		{
			shadowPass.reset(new Pass("ShadowPass"));
			shadowPass->setRenderTarget(renderTargets["shadow"]);
			shadowPass->setShader(shaders["shadow"]);
			renderGraph->addPass(shadowPass);
		}
		{
            blurPass.reset(new FullscreenPass("GaussianBlurPass"));
            blurPass->setRenderTarget(renderTargets["blur"]);
            blurPass->setShader(shaders["blur"]);
            blurPass->setTexture(0, textures["shadowColorTexture"]);
			renderGraph->addPass(blurPass);
		}
        {
            lightGBufferPass.reset(new FullscreenPass("LightPass"));
            lightGBufferPass->setRenderTarget(renderTargets["light"]);
            
            lightGBufferPass->setShader(shaders["light"]);
            lightGBufferPass->setTexture(0, textures["positions"]);
            lightGBufferPass->setTexture(1, textures["normals"]);
            lightGBufferPass->setTexture(2, textures["diffuse"]);
			lightGBufferPass->setTexture(3, textures["bluredShadowDepthTexture"]);
			lightGBufferPass->setTexture(4, textures["renderTargetDepthTexture"]);
            renderGraph->addPass(lightGBufferPass);
		}
		{
			testRenderPass.reset(new FullscreenPass("MainPass"));
			testRenderPass->setRenderTarget(mainRenderTarget);
			testRenderPass->setShader(shaderPipeline);
			testRenderPass->setTexture(0, textures["rtt"]);
			renderGraph->addPass(testRenderPass);
		}

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
	}

	void update(double dt) {
		mainRenderTarget->clear(context);

        for (auto& it : renderTargets) {
            it.second->clear(context);
        }
		shaderInputLayout->bind(context);
		//shaderInputLayout->bind();

        //lightCamera.m_Position.x = sin(glfwGetTime()) * 3.0f;
        //lightCamera.m_Position.z = cos(glfwGetTime()) * 2.0f;
        //lightCamera.m_Position.y = 3.0 + cos(glfwGetTime()) * 1.0f;

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = -20.0f, far_plane = 20.0f;
        lightProjection = lightCamera.getPerspectiveMatrix();
        lightView = glm::lookAt(glm::vec3(lightCamera.m_Position), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = glm::transpose(lightProjection * lightView);

        if (!useDebugCamera) {
			registry->view<CameraComponent, PlayerComponent>().each([&](CameraComponent& camera, PlayerComponent& player) {
				projectionData.projection = glm::transpose(camera.projectionMatrix * camera.viewMatrix);
			});
        } else {
			projectionData.projection = glm::transpose(camera.getPerspectiveMatrix() * camera.getViewMatrix());
        }
        
		projectionData.shadowProjection = lightSpaceMatrix;
        projectionData.viewPos = camera.m_Position;
        projectionData.shadowCameraPos = lightCamera.m_Position;
        //glm::transpose(lightCamera.getPerspectiveMatrix() * lightCamera.getViewMatrix());
		
        gbufferPass->addCommand(std::bind([&](mvp projectionData) {
            shaderBuffer->update(&projectionData);
            shaderBuffer->bind(context);
        }, projectionData));
        
		projectionData.projection = lightSpaceMatrix;
        shadowPass->addCommand(std::bind([&](mvp projectionData) {
            shaderBuffer->update(&projectionData);
            shaderBuffer->bind(context);
        }, projectionData));

        shadowPass->addCommand([]() {
            //glCullFace(GL_FRONT);
            glViewport(0, 0, 1024, 1024);
        });

        blurPass->addCommand([]() {
			//glCullFace(GL_BACK);
		});

        lightGBufferPass->addCommand([&]() {
            glViewport(0, 0, 1920, 1080);
        });

        registry->view<Transform, Render>().each([&](Transform& transform, Render& render) {
			modelData.toWorld = glm::mat4(1.0f);
			modelData.toWorld = glm::translate(modelData.toWorld, transform.translation);
			//modelData.toWorld = modelData.toWorld * glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
			modelData.toWorld = modelData.toWorld * glm::toMat4(transform.rotationq);
			modelData.toWorld = glm::transpose(glm::scale(modelData.toWorld, transform.scale));
			//modelData.toWorld = glm::transpose(transform.matrix);
			modelData.inverseToWorld = glm::inverse(modelData.toWorld);

			std::function<void()> command = std::bind([](ModelData modelData, ConstantBuffer* buffer) {
				Renderer* tempRenderer = Renderer::instance();

				buffer->update(&modelData);
				buffer->bind(tempRenderer->getContext());
				}, modelData, modelShaderBuffer.get());

			gbufferPass->addCommand(command);
			shadowPass->addCommand(command);

			this->updateTransform(render.model->getRootNode(), render.model.get(), glm::mat4(1.0f));

			for (auto& node : render.model->getNodes()) {
				if (node->mesh == -1) continue;

				std::function<void()> command = std::bind([](Model::Node* node, Model* model, ConstantBuffer* buffer) {
					Renderer* tempRenderer = Renderer::instance();
					const std::vector<std::shared_ptr<Model::SubMesh>>& submeshes = model->getSubMeshes();
					const std::shared_ptr<Model::SubMesh>& submesh = submeshes[node->mesh];

					MeshData meshData;
					meshData.transform = glm::transpose(node->transform.worldTransform);
					meshData.normalTransform = glm::transpose(node->transform.normalTransform);

					buffer->update(&meshData);
					buffer->bind(tempRenderer->getContext());

					submesh->vBuffer->bind(tempRenderer->getContext());
					submesh->iBuffer->bind(tempRenderer->getContext());

					tempRenderer->drawIndexed(submesh->iBuffer->getSize());
				}, node.get(), render.model.get(), meshShaderBuffer.get());

				gbufferPass->addCommand(command);
				shadowPass->addCommand(command);
			}
        });
        
        gbufferPass->addCommand(std::bind([&](PhysicsSystem* physicsSystem, std::shared_ptr<ShaderPipeline>& shader) {
            shader->bind(Renderer::instance()->getContext());
            physicsSystem->dynamicsWorld->debugDrawWorld();
		}, physicsSystem, shaders["physdebug"]));

        renderGraph->execute();
        renderer->bindMainRenderTarget();

        updateCamera();
	}

    void updateCamera() {
		if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_UP)) {
			lightCamera.m_Position.x += 0.2f;
		} else if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_DOWN)) {
			lightCamera.m_Position.x -= 0.2f;
		}

		if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_LEFT)) {
			lightCamera.m_Position.z += 0.2f;
		} else if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_RIGHT)) {
			lightCamera.m_Position.z -= 0.2f;
		}

        if (!useDebugCamera) return;
        if (InputManager::instance()->isKeyPressed(GLFW_KEY_1)) {
            mainCamera = 0;
        } else if (InputManager::instance()->isKeyPressed(GLFW_KEY_2)) {
            mainCamera = 1;
        }

        if (InputManager::instance()->isKeyPressed(GLFW_KEY_E)) {
            camera.m_Rotation.x -= (float)InputManager::instance()->mouseMoveX * 0.0045f;
            camera.m_Rotation.y += (float)InputManager::instance()->mouseMoveY * 0.0045f;
        }

        if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_W)) {
            camera.m_Position += camera.forwardVec * 0.2f;
        } else if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_S)) {
            camera.m_Position -= camera.forwardVec * 0.2f;
        }

        if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_A)) {
            camera.m_Position += camera.rightVec * 0.2f;
        } else if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_D)) {
            camera.m_Position -= camera.rightVec * 0.2f;
        }   

        camera.update();
    }

    void updateTransform(const int nodeId, Model* renderable, const glm::mat4& parentTransform)
    {
        const std::vector<std::shared_ptr<Model::Node>>& nodes = renderable->getNodes();
        if (nodes.size() <= nodeId) return;

        const std::shared_ptr<Model::Node>& node = nodes[nodeId];

        if (node->transform.hasChanged == true) {
            glm::mat4 localTransform = glm::mat4(1.0f);
            localTransform = glm::translate(localTransform, node->transform.translation);
            localTransform = localTransform * glm::toMat4(node->transform.rotation);
            localTransform = glm::scale(localTransform, node->transform.scale);

            node->transform.worldTransform = parentTransform * localTransform;
            node->transform.normalTransform = glm::inverse(node->transform.worldTransform);
            node->transform.hasChanged = false;
        }

        for (auto& it : node->childs) {
            updateTransform(it, renderable, node->transform.worldTransform);
        }
    }

    void createShaderFromFolder(const std::string& folder, const std::string& name) {
		std::string vertexShaderFilename;
		std::string pixelShaderFilename;
		std::string geometryShaderFilename;

        if (Renderer::getType() == RendererType::OpenGL) {
            vertexShaderFilename = "/pixel.glsl";
            pixelShaderFilename = "/vertex.glsl";
            geometryShaderFilename = "/geometry.glsl";
        } else {
			vertexShaderFilename = "/pixel.hlsl";
			pixelShaderFilename = "/vertex.hlsl";
			geometryShaderFilename = "/geometry.hlsl";
        }

		File filePS(folder + pixelShaderFilename);
		File fileVS(folder + vertexShaderFilename);
		File fileGS(folder + geometryShaderFilename);

        if (!filePS.exists() || !fileVS.exists()) {
            std::cout << "Could not create shader from path " << folder << std::endl;
            return;
        }

        shaders[name] = std::shared_ptr<ShaderPipeline>(ShaderPipeline::create(fileVS.getConent(), filePS.getConent()));
    }
};