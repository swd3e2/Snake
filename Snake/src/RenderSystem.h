#pragma once

#include "Input/InputManager.h"
#include <GL/glew.h>
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

struct Drawable {
    VertexBuffer* vb;
    IndexBuffer* ib;
    glm::mat4 transform;
    glm::mat4 nodeTransform;
    glm::mat4 normalTransform;
};

class RenderSystem {
private:
    struct mvp {
        glm::mat4 projection;
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
    std::queue<std::function<void()>> commands;
    std::queue<Drawable> drawables;
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
    std::shared_ptr<RenderTarget> rt;
    std::shared_ptr<RenderTarget> rt2;
    std::shared_ptr<MainRenderTarget> mainRenderTarget;
    std::shared_ptr<Texture2D> noise;
    entt::registry* registry;
    std::shared_ptr<Texture2D> positions;
    std::shared_ptr<Texture2D> normals;
    std::shared_ptr<Texture2D> diffuse;
    
    float cameraX = -1.5f;
    float cameraY = -1.5f;

    double cameraRotX = 0.0f;
    double cameraRotY = 0.0f;
    std::shared_ptr<Texture2D> tex;
    Camera camera;

    std::shared_ptr<FullscreenPass> lightGBufferPass;
    std::shared_ptr<FullscreenPass> testRenderPass;
    std::shared_ptr<Pass> forwardRenderPass;
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
        shaderInputLayout->add({ InputDataType::Float4, "weigth" });

        std::vector<vertex> data;
        data.push_back(vertex(-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f));
        data.push_back(vertex( 0.0f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f));
        data.push_back(vertex( 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f));

        vertexBuffer.reset(VertexBuffer::create(data.size(), sizeof(vertex), data.data()));

        shaderBuffer.reset(ConstantBuffer::create(0, sizeof(projectionData), &projectionData));
        modelShaderBuffer.reset(ConstantBuffer::create(1, sizeof(modelData), &modelData));
        meshShaderBuffer.reset(ConstantBuffer::create(2, sizeof(meshData), &meshData));
        materialShaderBuffer.reset(ConstantBuffer::create(3, sizeof(materialData), &materialData));

        std::shared_ptr<RawTexture> texture = std::make_shared<RawTexture>("texture_01.png");
        texture->import();
        tex.reset(Texture2D::create(texture->getWidth(), texture->getHeight(), 0, texture->getData(), texture->getChannels() == 4 ? TextureFormat::RGBA8 : TextureFormat::RGB8));

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        camera.setProjectionMatrix(90.0f, 1920.0f / 1080.0f, 0.1f, 1000.f);

        glEnable(GL_DEPTH_TEST);

        mainRenderTarget.reset(Renderer::getMainRenderTarget());

        renderGraph = std::make_unique<RenderGraph>(&renderer);

        size_t width = 100;
        size_t height = 100;
        noise.reset(Texture2D::create(width, height, 4, nullptr, TextureFormat::R8, 4));

        char* rawdata = new char[width * height * 4];
        for (size_t x = 0; x < height; x++) {
            for (size_t y = 0; y < width; y++) {
                rawdata[x * width*4 + y*4] = 255;
                rawdata[x * width*4 + y*4 + 1] = 255;
                rawdata[x * width*4 + y*4 + 2] = 255;
                rawdata[x * width*4 + y*4 + 3] = 255;
            }
        }
        noise->setData(rawdata);
        delete[] rawdata;
        noise->generateMips();
        
    
        std::shared_ptr<Texture2D> renderTargetDepthTexture;
        {
            File fileVS("shaders/gbuffer/vertex.glsl");
            File filePS("shaders/gbuffer/pixel.glsl");

            std::shared_ptr<ShaderPipeline> defferedShader;
            defferedShader.reset(ShaderPipeline::create(fileVS.getConent(), filePS.getConent()));
            
            positions.reset(Texture2D::create(1920, 1080, 0, nullptr, TextureFormat::RGBA32, 6));
            positions->generateMips();
            normals.reset(Texture2D::create(1920, 1080, 1, nullptr, TextureFormat::RGBA32, 6));
            normals->generateMips();
            diffuse.reset(Texture2D::create(1920, 1080, 2, nullptr, TextureFormat::RGBA32, 6));
            diffuse->generateMips();

            renderTargetDepthTexture.reset(Texture2D::create(1920, 1080, 3, nullptr, TextureFormat::D24S8, 6));

            rt.reset(RenderTarget::create());
            rt->setColorTexture(positions, 0, 1);
            rt->setColorTexture(normals, 1, 1);
            rt->setColorTexture(diffuse, 2, 1);
            rt->setDepthTexture(renderTargetDepthTexture, 1);

            forwardRenderPass.reset(new Pass("forward"));
            forwardRenderPass->bindables.push_back(rt);
            forwardRenderPass->bindables.push_back(defferedShader);
            forwardRenderPass->bindables.push_back(tex);
            renderGraph->addPass(forwardRenderPass);
        }
        std::shared_ptr<Texture2D> rtt;
        {
            File fileVS("shaders/smaa/vertex.glsl");
            File filePS("shaders/smaa/pixel.glsl");

            std::shared_ptr<ShaderPipeline> smaaShader;
            smaaShader.reset(ShaderPipeline::create(fileVS.getConent(), filePS.getConent()));

            rtt.reset(Texture2D::create(1920, 1080, 0, nullptr, TextureFormat::RGBA32));
            std::shared_ptr<Texture2D> rtdt;
            rtdt.reset(Texture2D::create(1920, 1080, 2, nullptr, TextureFormat::D24S8));
            rt2.reset(RenderTarget::create());
            rt2->setColorTexture(rtt, 0, 0);
            rt2->setDepthTexture(rtdt, 0);

            lightGBufferPass.reset(new FullscreenPass("test"));
            lightGBufferPass->bindables.push_back(rt2);
            lightGBufferPass->bindables.push_back(smaaShader);
            lightGBufferPass->bindables.push_back(positions);
            lightGBufferPass->bindables.push_back(normals);
            lightGBufferPass->bindables.push_back(diffuse);
            lightGBufferPass->bindables.push_back(renderTargetDepthTexture);
            lightGBufferPass->bindables.push_back(noise);
            renderGraph->addPass(lightGBufferPass);
        }
        {
            testRenderPass.reset(new FullscreenPass("test2"));
            testRenderPass->bindables.push_back(shaderPipeline);
            testRenderPass->bindables.push_back(mainRenderTarget);
            testRenderPass->bindables.push_back(rtt);
            renderGraph->addPass(testRenderPass);
        }
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
	}

	void update(double dt) {
        renderer.bindMainRenderTarget();

        rt->clear(context);
        rt2->clear(context);
        shaderInputLayout->bind();

		projectionData.projection = glm::transpose(camera.getPerspectiveMatrix() * camera.getViewMatrix());
		
        forwardRenderPass->addCommand(std::bind([&](mvp projectionData) {
            shaderBuffer->update(&projectionData);
            shaderBuffer->bind(context);
        }, projectionData));
        
        materialData.color = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
        materialShaderBuffer->update(&materialData);
        materialShaderBuffer->bind(context);

        forwardRenderPass->addCommand([&]() {
            glViewport(0, 0, 960, 540);
        });

        registry->view<Transform, Render>().each([&](Transform& transform, Render& render) {
            modelData.toWorld = glm::mat4(1.0f);
            modelData.toWorld = glm::translate(modelData.toWorld, transform.translation);
            modelData.toWorld = modelData.toWorld * glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
            modelData.toWorld = glm::transpose(glm::scale(modelData.toWorld, transform.scale));
            //modelData.toWorld = glm::transpose(transform.matrix);
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
            }
        });

        lightGBufferPass->addCommand([&]() {
            renderer.bindMainRenderTarget();
            //positions->generateMips();
            //diffuse->generateMips();
            glViewport(0, 0, 1920, 1080);
        });

        renderGraph->execute();
        renderer.bindMainRenderTarget();

        updateCamera();
	}

    void updateCamera()
    {
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
            node->transform.matrixTransform = glm::inverse(node->transform.worldTransform);
            node->transform.hasChanged = false;
        }

        for (auto& it : node->childs) {
            updateTransform(it, renderable, node->transform.worldTransform);
        }
    }
};