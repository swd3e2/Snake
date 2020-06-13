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
    struct MeshData {
        glm::mat4 transform;
        glm::mat4 matrixTransform;
    };
    struct MaterialData {
        glm::vec4 color;
    };
public:
    mvp projectionData;
    ModelData modelData;
    MeshData meshData;
    MaterialData materialData;
    OpenGlRenderer renderer;
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
    std::shared_ptr<Pass> forwardRenderPass;
    std::shared_ptr<Pass> shadowPass;
    int mainCamera = 0;

    std::unordered_map<std::string, std::shared_ptr<Texture2D>> textures;
    std::unordered_map<std::string, std::shared_ptr<RenderTarget>> renderTargets;
    std::unordered_map<std::string, std::shared_ptr<ConstantBuffer>> constantBuffers;
    std::unordered_map<std::string, std::shared_ptr<ShaderPipeline>> shaders;

	std::shared_ptr<RenderTarget> rt;
	std::shared_ptr<RenderTarget> rt2;
	std::shared_ptr<RenderTarget> shadowRt;
public:
	void init(entt::registry* registry) {
        this->registry = registry;
        context = renderer.getContext();

        File fileVS("defaultVS.glsl");
        File filePS("defaultPS.glsl");

        shaderPipeline.reset(ShaderPipeline::create(fileVS.getConent(), filePS.getConent()));

        shaderInputLayout = std::make_shared<ShaderInputLayout>();

        shaderInputLayout->add({ InputDataType::Float3, "pos" });
        shaderInputLayout->add({ InputDataType::Float2, "texCoords" });
        shaderInputLayout->add({ InputDataType::Float3, "normal" });
        shaderInputLayout->add({ InputDataType::Float3, "tangent" });
        shaderInputLayout->add({ InputDataType::Float3, "bitangent" });
        shaderInputLayout->add({ InputDataType::Int4,   "bone" });
        shaderInputLayout->add({ InputDataType::Float4, "weight" });

        shaderBuffer.reset(ConstantBuffer::create(0, sizeof(projectionData), nullptr));
        modelShaderBuffer.reset(ConstantBuffer::create(1, sizeof(modelData), nullptr));
        meshShaderBuffer.reset(ConstantBuffer::create(2, sizeof(meshData), nullptr));
        materialShaderBuffer.reset(ConstantBuffer::create(3, sizeof(materialData), nullptr));

        std::shared_ptr<RawTexture> texture = std::make_shared<RawTexture>("texture_01.png");
        texture->import();
        textures["tex"] = std::shared_ptr<Texture2D>(Texture2D::create(texture->getWidth(), texture->getHeight(), 0, texture->getData(), texture->getChannels() == 4 ? TextureFormat::RGBA8 : TextureFormat::RGB8));

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        camera.setProjectionMatrix(90.0f, 1920.0f / 1080.0f, 0.1f, 200.f);
        lightCamera.setOrthographicMatrix(-10, 10, -10, 10, -10, 10);
        //lightCamera.m_Rotation = glm::vec4(0.0f, 0.5f, 0.0f, 1.0f);
        lightCamera.m_Position = glm::vec4(7.0f, 4.0f, -2.0f, 1.0f);

        glEnable(GL_DEPTH_TEST);

        mainRenderTarget.reset(Renderer::getMainRenderTarget());

        renderGraph = std::make_unique<RenderGraph>(&renderer);


        // Textures
        {
			textures["positions"] = std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 0, nullptr, TextureFormat::RGBA32));
			textures["normals"] = std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 1, nullptr, TextureFormat::RGBA32));
			textures["diffuse"] = std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 2, nullptr, TextureFormat::RGBA32));
			textures["renderTargetDepthTexture"] = std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 3, nullptr, TextureFormat::D24S8));
			textures["rtt"] = std::shared_ptr<Texture2D>(Texture2D::create(1920, 1080, 0, nullptr, TextureFormat::RGBA32));
			textures["shadowDepthTexture"] = std::shared_ptr<Texture2D>(Texture2D::create(2048, 2048, 5, nullptr, TextureFormat::D32));
        }
        // Shaders
        {
			createShaderFromFolder("shaders/gbuffer", "gbuffer");
			createShaderFromFolder("shaders/shadow", "shadow");
			createShaderFromFolder("shaders/smaa", "light");
        }
        // Render targets
        {
            {
				std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
				renderTarget->setColorTexture(textures["positions"], 0, 0);
				renderTarget->setColorTexture(textures["normals"], 1, 0);
				renderTarget->setColorTexture(textures["diffuse"], 2, 0);
				renderTarget->setDepthTexture(textures["renderTargetDepthTexture"], 0);
				renderTargets["gBuffer"] = renderTarget;
			}
			{
				std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
				renderTarget->setDepthTexture(textures["shadowDepthTexture"], 0);
				renderTargets["shadow"] = renderTarget;
			}
			{
				std::shared_ptr<RenderTarget> renderTarget = std::shared_ptr<RenderTarget>(RenderTarget::create());
				renderTarget->setColorTexture(textures["rtt"], 0, 0);
				renderTargets["light"] = renderTarget;
			}
        }
        // Render passess
        {
            forwardRenderPass.reset(new Pass("forward"));
            forwardRenderPass->setRenderTarget(renderTargets["gBuffer"]);

            forwardRenderPass->bindables.push_back(shaders["gbuffer"]);
            forwardRenderPass->bindables.push_back(textures["tex"]);
            renderGraph->addPass(forwardRenderPass);
        }
        {
            shadowPass.reset(new Pass("forward"));
            shadowPass->setRenderTarget(renderTargets["shadow"]);

            shadowPass->bindables.push_back(shaders["shadow"]);
            renderGraph->addPass(shadowPass);
        }
        {
            lightGBufferPass.reset(new FullscreenPass("light"));
            lightGBufferPass->setRenderTarget(renderTargets["light"]);
            
            lightGBufferPass->bindables.push_back(shaders["light"]);
            lightGBufferPass->bindables.push_back(textures["positions"]);
            lightGBufferPass->bindables.push_back(textures["normals"]);
            lightGBufferPass->bindables.push_back(textures["diffuse"]);
            lightGBufferPass->bindables.push_back(textures["renderTargetDepthTexture"]);
            lightGBufferPass->bindables.push_back(textures["shadowDepthTexture"]);
            renderGraph->addPass(lightGBufferPass);
        }
        {
            testRenderPass.reset(new FullscreenPass("main"));
            testRenderPass->setRenderTarget(mainRenderTarget);

            testRenderPass->bindables.push_back(shaderPipeline);
            testRenderPass->bindables.push_back(textures["rtt"]);
            renderGraph->addPass(testRenderPass);
        }

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
	}

	void update(double dt) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		renderer.bindMainRenderTarget();
            
        for (auto& it : renderTargets) {
            it.second->clear(context);
        }
        shaderInputLayout->bind();

        //lightCamera.m_Position.x = sin(glfwGetTime()) * 3.0f;
        //lightCamera.m_Position.z = cos(glfwGetTime()) * 2.0f;
        //lightCamera.m_Position.y = 3.0 + cos(glfwGetTime()) * 1.0f;

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = -20.0f, far_plane = 20.0f;
        lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
        lightView = glm::lookAt(glm::vec3(lightCamera.m_Position), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = glm::transpose(lightProjection * lightView);

		projectionData.projection = glm::transpose(camera.getPerspectiveMatrix() * camera.getViewMatrix());
		projectionData.shadowProjection = lightSpaceMatrix;
        projectionData.viewPos = camera.m_Position;
        projectionData.shadowCameraPos = lightCamera.m_Position;
        //glm::transpose(lightCamera.getPerspectiveMatrix() * lightCamera.getViewMatrix());
		
        forwardRenderPass->addCommand(std::bind([&](mvp projectionData) {
            shaderBuffer->update(&projectionData);
            shaderBuffer->bind(context);
        }, projectionData));
        
		projectionData.projection = lightSpaceMatrix;
        shadowPass->addCommand(std::bind([&](mvp projectionData) {
            shaderBuffer->update(&projectionData);
            shaderBuffer->bind(context);
        }, projectionData));

        materialData.color = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
        materialShaderBuffer->update(&materialData);
        materialShaderBuffer->bind(context);

        shadowPass->addCommand([]() {
            glCullFace(GL_FRONT);
            glViewport(0, 0, 2048, 2048);
        });

        lightGBufferPass->addCommand([&]() {
            glCullFace(GL_BACK);
            glViewport(0, 0, 1920, 1080);
        });

        registry->view<Transform, Render>().each([&](Transform& transform, Render& render) {
            modelData.toWorld = glm::mat4(1.0f);
            modelData.toWorld = glm::translate(modelData.toWorld, transform.translation);
            modelData.toWorld = modelData.toWorld * glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
            modelData.toWorld = glm::transpose(glm::scale(modelData.toWorld, transform.scale));
            modelData.toWorld = glm::transpose(transform.matrix);
            modelData.inverseToWorld = glm::inverse(modelData.toWorld);

            std::function<void()> command = std::bind([](ModelData modelData, ConstantBuffer* buffer) {
                Renderer* tempRenderer = Renderer::instance();

                buffer->update(&modelData);
                buffer->bind(tempRenderer->getContext());
            }, modelData, modelShaderBuffer.get());
            renderGraph->addCommand(command);

            this->updateTransform(render.model->getRootNode(), render.model.get(), glm::mat4(1.0f));

            for (auto& node : render.model->getNodes()) {
                if (node->mesh == -1) continue;
          
                std::function<void()> command = std::bind([](Model::Node* node, Model* model, ConstantBuffer* buffer) {
                    Renderer* tempRenderer = Renderer::instance();
                    std::vector<std::shared_ptr<Model::SubMesh>> submeshes = model->getSubMeshes();

                    const std::shared_ptr<Model::SubMesh>& submesh = submeshes[node->mesh];
                    MeshData meshData;
                    meshData.transform = glm::transpose(node->transform.worldTransform);
                    meshData.matrixTransform = glm::transpose(node->transform.matrixTransform);

                    buffer->update(&meshData);
                    buffer->bind(tempRenderer->getContext());
                    
                    submesh->vBuffer->bind(tempRenderer->getContext());
                    submesh->iBuffer->bind(tempRenderer->getContext());

                    tempRenderer->drawIndexed(submesh->iBuffer->getSize());
                }, node.get(), render.model.get(), meshShaderBuffer.get());

                forwardRenderPass->addCommand(command);
                shadowPass->addCommand(command);
            }
        });

        renderGraph->execute();
        renderer.bindMainRenderTarget();

        updateCamera();
	}

    void updateCamera()
    {
        if (InputManager::instance()->isKeyPressed(GLFW_KEY_1)) {
            mainCamera = 0;
        } else if (InputManager::instance()->isKeyPressed(GLFW_KEY_2)) {
            mainCamera = 1;
        }

        Camera* selectedCamera = nullptr;

        if (mainCamera == 0) {
            selectedCamera = &camera;
        } else if (mainCamera == 1) {
            selectedCamera = &lightCamera;
        }

        if (selectedCamera == nullptr) return;

        if (InputManager::instance()->isKeyPressed(GLFW_KEY_E)) {
            selectedCamera->m_Rotation.x -= (float)InputManager::instance()->mouseMoveX * 0.0045f;
            selectedCamera->m_Rotation.y += (float)InputManager::instance()->mouseMoveY * 0.0045f;
        }

        if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_W)) {
            selectedCamera->m_Position += selectedCamera->forwardVec * 0.2f;
        } else if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_S)) {
            selectedCamera->m_Position -= selectedCamera->forwardVec * 0.2f;
        }

        if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_A)) {
            selectedCamera->m_Position += selectedCamera->rightVec * 0.2f;
        } else if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_D)) {
            selectedCamera->m_Position -= selectedCamera->rightVec * 0.2f;
        }   

        if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_UP)) {
            lightCamera.m_Position.x +=  0.2f;
        }
        else if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_DOWN)) {
            lightCamera.m_Position.x -= 0.2f;
        }

        if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_LEFT)) {
            lightCamera.m_Position.z += 0.2f;
        }
        else if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_RIGHT)) {
            lightCamera.m_Position.z -= 0.2f;
        }

        camera.update();
        //lightCamera.update();
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
            node->transform.matrixTransform = glm::inverse(node->transform.worldTransform);
            node->transform.hasChanged = false;
        }

        for (auto& it : node->childs) {
            updateTransform(it, renderable, node->transform.worldTransform);
        }
    }

    void createShaderFromFolder(const std::string& folder, const std::string& name) {
        File filePS(folder + "/pixel.glsl");
        File fileVS(folder + "/vertex.glsl");
        File fileGS(folder + "/geometry.glsl");

        if (!filePS.exists() || !fileVS.exists()) {
            std::cout << "Could not create shader from path " << folder << std::endl;
            return;
        }

        shaders[name] = std::shared_ptr<ShaderPipeline>(ShaderPipeline::create(fileVS.getConent(), filePS.getConent()));
    }
};