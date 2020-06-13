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
#include <glm/glm.hpp>
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
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

// meshes
unsigned int planeVAO;
void createPlane() {
    // ------------------------------------------------------------------
    float planeVertices[] = {
     // positions            // normals         // texcoords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };
    // plane VAO
    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left             
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

// renders the 3D scene
// --------------------
void renderScene(ModelData& modelData, const std::shared_ptr<ConstantBuffer>& modelShaderBuffer)
{
    // floor
    glm::mat4 model = glm::mat4(1.0f);
    modelData.toWorld = model;
    modelShaderBuffer->update(&modelData);
    modelShaderBuffer->bind(Renderer::instance()->getContext());
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    modelData.toWorld = glm::transpose(model);
    modelShaderBuffer->update(&modelData);
    modelShaderBuffer->bind(Renderer::instance()->getContext());
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.1f));
    modelData.toWorld = glm::transpose(model);
    modelShaderBuffer->update(&modelData);
    modelShaderBuffer->bind(Renderer::instance()->getContext());
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.25));
    modelData.toWorld = glm::transpose(model);
    modelShaderBuffer->update(&modelData);
    modelShaderBuffer->bind(Renderer::instance()->getContext());
    renderCube();
}

struct Drawable {
    VertexBuffer* vb;
    IndexBuffer* ib;
    glm::mat4 transform;
    glm::mat4 nodeTransform;
    glm::mat4 normalTransform;
};

class RenderSystem {
private:
    
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
    entt::registry* registry;

    std::shared_ptr<ShaderPipeline> shaderPipeline;
    std::shared_ptr<ShaderInputLayout> shaderInputLayout;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<ConstantBuffer> shaderBuffer;
    std::shared_ptr<ConstantBuffer> modelShaderBuffer;
    std::shared_ptr<ConstantBuffer> meshShaderBuffer;
    std::shared_ptr<ConstantBuffer> materialShaderBuffer;
    std::shared_ptr<RenderTarget> rt;
    std::shared_ptr<RenderTarget> rt2;
    std::shared_ptr<RenderTarget> shadowRt;
    std::shared_ptr<MainRenderTarget> mainRenderTarget;
    std::shared_ptr<Texture2D> noise;
    std::shared_ptr<Texture2D> positions;
    std::shared_ptr<Texture2D> normals;
    std::shared_ptr<Texture2D> diffuse;
    std::shared_ptr<Texture2D> shadowColorTexture;
    std::shared_ptr<Texture2D> shadowDepthTexture;
    
    std::shared_ptr<Texture2D> tex;
    Camera camera;
    Camera lightCamera;
    glm::vec3 lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
    std::shared_ptr<FullscreenPass> lightGBufferPass;
    std::shared_ptr<Pass> testRenderPass;
    std::shared_ptr<Pass> forwardRenderPass;
    std::shared_ptr<Pass> shadowPass;
    int mainCamera = 0;
public:
	void init(entt::registry* registry) {
        this->registry = registry;
        context = renderer.getContext();

        File fileVS("defaultVS.glsl");
        File filePS("defaultPS.glsl");

        shaderPipeline.reset(ShaderPipeline::create(fileVS.getConent(), filePS.getConent()));

        // shaderInputLayout = std::make_shared<ShaderInputLayout>();

        // shaderInputLayout->add({ InputDataType::Float3, "pos" });
        // shaderInputLayout->add({ InputDataType::Float2, "texCoords" });
        // shaderInputLayout->add({ InputDataType::Float3, "normal" });
        // shaderInputLayout->add({ InputDataType::Float3, "tangent" });
        // shaderInputLayout->add({ InputDataType::Float3, "bitangent" });
        // shaderInputLayout->add({ InputDataType::Int4,   "bone" });
        // shaderInputLayout->add({ InputDataType::Float4, "weigth" });

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

        camera.setProjectionMatrix(90.0f, 1920.0f / 1080.0f, 0.1f, 100.f);
        lightCamera.setOrthographicMatrix(-20.0f, 20.0f, -20.0f, 20.0f, -20.0f, 20.0f);
        lightCamera.m_Position = glm::vec4(-2.0f, 4.0f, -1.0f, 1.0f);
        lightCamera.m_Rotation = glm::vec4(0.0f, 0.5f, 0.0f, 1.0f);

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
            
            positions.reset(Texture2D::create(1920, 1080, 0, nullptr, TextureFormat::RGBA32));
            normals.reset(Texture2D::create(1920, 1080, 1, nullptr, TextureFormat::RGBA32));
            diffuse.reset(Texture2D::create(1920, 1080, 2, nullptr, TextureFormat::RGBA32));

            renderTargetDepthTexture.reset(Texture2D::create(1920, 1080, 3, nullptr, TextureFormat::D24S8));

            rt.reset(RenderTarget::create());
            rt->setColorTexture(positions, 0, 0);
            rt->setColorTexture(normals, 1, 0);
            rt->setColorTexture(diffuse, 2, 0);
            rt->setDepthTexture(renderTargetDepthTexture, 0);

            forwardRenderPass.reset(new Pass("forward"));
            forwardRenderPass->bindables.push_back(rt);
            forwardRenderPass->bindables.push_back(defferedShader);
            forwardRenderPass->bindables.push_back(tex);
            renderGraph->addPass(forwardRenderPass);
        }
        {
            File fileVS("shaders/shadow/vertex.glsl");
            File filePS("shaders/shadow/pixel.glsl");

            std::shared_ptr<ShaderPipeline> shadowShader;
            shadowShader.reset(ShaderPipeline::create(fileVS.getConent(), filePS.getConent()));
            
            shadowColorTexture.reset(Texture2D::create(2048, 2048, 5, nullptr, TextureFormat::RGB8));
            shadowDepthTexture.reset(Texture2D::create(2048, 2048, 5, nullptr, TextureFormat::D32));

            shadowRt.reset(RenderTarget::create());
            //shadowRt->setColorTexture(shadowColorTexture, 0, 0);
            shadowRt->setDepthTexture(shadowDepthTexture, 0);

            shadowPass.reset(new Pass("forward"));
            shadowPass->bindables.push_back(shadowRt);
            shadowPass->bindables.push_back(shadowShader);
            renderGraph->addPass(shadowPass);
        }
        std::shared_ptr<Texture2D> rtt;
        {
            File fileVS("shaders/smaa/vertex.glsl");
            File filePS("shaders/smaa/pixel.glsl");

            std::shared_ptr<ShaderPipeline> smaaShader;
            smaaShader.reset(ShaderPipeline::create(fileVS.getConent(), filePS.getConent()));

            rtt.reset(Texture2D::create(1920, 1080, 0, nullptr, TextureFormat::RGBA32));
            std::shared_ptr<Texture2D> rtdt;
            rtdt.reset(Texture2D::create(1920, 1080, 2, nullptr, TextureFormat::D32));
            rt2.reset(RenderTarget::create());
            rt2->setColorTexture(rtt, 0, 0);
            rt2->setDepthTexture(rtdt, 0);

            lightGBufferPass.reset(new FullscreenPass("test"));
            // lightGBufferPass->bindables.push_back(rt2);
            // lightGBufferPass->bindables.push_back(smaaShader);
            // lightGBufferPass->bindables.push_back(positions);
            // lightGBufferPass->bindables.push_back(normals);
            // lightGBufferPass->bindables.push_back(diffuse);
            // lightGBufferPass->bindables.push_back(renderTargetDepthTexture);
            // lightGBufferPass->bindables.push_back(noise);
            // lightGBufferPass->bindables.push_back(shadowDepthTexture);
            //renderGraph->addPass(lightGBufferPass);
        }
        {
            testRenderPass.reset(new Pass("test2"));
            testRenderPass->bindables.push_back(shaderPipeline);
            testRenderPass->bindables.push_back(mainRenderTarget);
            testRenderPass->bindables.push_back(rtt);
            testRenderPass->bindables.push_back(tex);
            testRenderPass->bindables.push_back(shadowDepthTexture);
            renderGraph->addPass(testRenderPass);
        }
        createPlane();
       // glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
	}

	void update(double dt) {
        lightCamera.m_Rotation.x += dt * 0.001f;
        rt->clear(context);
        rt2->clear(context);
        shadowRt->clear(context);
        tex->bind(context);
        //shaderInputLayout->bind();
        {
            lightPos.x = sin(glfwGetTime()) * 3.0f;
            lightPos.z = cos(glfwGetTime()) * 2.0f;
            lightPos.y = 3.0 + cos(glfwGetTime()) * 1.0f;

            glm::mat4 lightProjection, lightView;
            glm::mat4 lightSpaceMatrix;
            float near_plane = -20.0f, far_plane = 20.0f;
            lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
            lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
            lightSpaceMatrix = lightProjection * lightView;

            projectionData.projection = glm::transpose(lightSpaceMatrix);
            projectionData.shadowCameraPos = glm::vec4(lightPos, 1.0f);
        }
        {
            lightCamera.m_Position.x = sin(glfwGetTime()) * 3.0f;
            lightCamera.m_Position.z = cos(glfwGetTime()) * 2.0f;
            lightCamera.m_Position.y = 3.0 + cos(glfwGetTime()) * 1.0f;

            glm::mat4 lightProjection, lightView;
            glm::mat4 lightSpaceMatrix;
            lightProjection = lightCamera.getPerspectiveMatrix();
            lightView = glm::lookAt(glm::vec3(lightCamera.m_Position), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
            lightSpaceMatrix = lightProjection * lightView;

            projectionData.projection = glm::transpose(lightSpaceMatrix);
            //projectionData.projection = glm::transpose(lightCamera.getPerspectiveMatrix() * lightCamera.getViewMatrix());
            projectionData.shadowCameraPos = lightCamera.m_Position;
        }
        projectionData.viewPos = lightCamera.m_Position;
            projectionData.shadowProjection = projectionData.projection;

        shaderPipeline->bind(context);
        {
            glViewport(0, 0, 2048, 2048);
            shadowRt->bind(context);
            //projectionData.projection = glm::transpose(lightCamera.getPerspectiveMatrix() * lightCamera.getViewMatrix());
            
            shaderBuffer->update(&projectionData);
            shaderBuffer->bind(context);
            renderScene(modelData, modelShaderBuffer);
        }
        {
            shadowDepthTexture->bind(context);
            glViewport(0, 0, 1920, 1080);
            renderer.bindMainRenderTarget();
            projectionData.projection = glm::transpose(camera.getPerspectiveMatrix() * camera.getViewMatrix());
            //projectionData.shadowProjection = glm::transpose(lightCamera.getPerspectiveMatrix() * lightCamera.getViewMatrix());

            shaderBuffer->update(&projectionData);
            shaderBuffer->bind(context);
            shaderPipeline->bind(context);
            renderScene(modelData, modelShaderBuffer);
        }
		
        

        // testRenderPass->addCommand(std::bind([&](mvp projectionData) {
        //     shaderBuffer->update(&projectionData);
        //     shaderBuffer->bind(context);
        // }, projectionData));
        
		// projectionData.projection = glm::transpose(lightCamera.getPerspectiveMatrix() * lightCamera.getViewMatrix());
        // shadowPass->addCommand(std::bind([&](mvp projectionData) {
        //     shaderBuffer->update(&projectionData);
        //     shaderBuffer->bind(context);
        // }, projectionData));

        // materialData.color = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
        // materialShaderBuffer->update(&materialData);
        // materialShaderBuffer->bind(context);
    
        // shadowPass->addCommand([]() {
        //     glCullFace(GL_BACK);
        //     glViewport(0, 0, 2048, 2048);
        // });

        // testRenderPass->addCommand([&]() {
        //     glCullFace(GL_BACK);
        //     glViewport(0, 0, 1920, 1080);
        // });

        // registry->view<Transform, Render>().each([&](Transform& transform, Render& render) {
        //     modelData.toWorld = glm::mat4(1.0f);
        //     modelData.toWorld = glm::translate(modelData.toWorld, transform.translation);
        //     modelData.toWorld = modelData.toWorld * glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
        //     modelData.toWorld = glm::transpose(glm::scale(modelData.toWorld, transform.scale));
        //     //modelData.toWorld = glm::transpose(transform.matrix);
        //     modelData.inverseToWorld = glm::inverse(modelData.toWorld);

        //     std::function<void()> command = std::bind([](ModelData modelData, ConstantBuffer* buffer) {
        //         Renderer* tempRenderer = Renderer::instance();

        //         buffer->update(&modelData);
        //         buffer->bind(tempRenderer->getContext());
        //     }, modelData, modelShaderBuffer.get());
        //     testRenderPass->addCommand(command);
        //     shadowPass->addCommand(command);

        //     this->updateTransform(render.model->getRootNode(), render.model.get(), glm::mat4(1.0f));

        //     for (auto& node : render.model->getNodes()) {
        //         if (node->mesh == -1) continue;
          
        //         std::function<void()> command = std::bind([](Model::Node* node, Model* model, ConstantBuffer* buffer) {
        //             Renderer* tempRenderer = Renderer::instance();
        //             std::vector<std::shared_ptr<Model::SubMesh>> submeshes = model->getSubMeshes();

        //             const std::shared_ptr<Model::SubMesh>& submesh = submeshes[node->mesh];
        //             MeshData meshData;
        //             meshData.transform = glm::transpose(node->transform.worldTransform);
        //             meshData.matrixTransform = glm::transpose(node->transform.matrixTransform);

        //             buffer->update(&meshData);
        //             buffer->bind(tempRenderer->getContext());
                    
        //             submesh->vBuffer->bind(tempRenderer->getContext());
        //             submesh->iBuffer->bind(tempRenderer->getContext());

        //             tempRenderer->drawIndexed(submesh->iBuffer->getSize());
        //         }, node.get(), render.model.get(), meshShaderBuffer.get());

        //         testRenderPass->addCommand(command);
        //         shadowPass->addCommand(command);
        //     }
        // });
        
        // testRenderPass->addCommand([&]() {
        //     renderer.bindMainRenderTarget();
        //     //positions->generateMips();
        //     //diffuse->generateMips();
        //     //glViewport(0, 0, 1920, 1080);
        // });

        //renderGraph->execute();

        updateCamera(dt);
	}

    void updateCamera(double dt)
    {
        if (InputManager::instance()->isKeyPressed(GLFW_KEY_1)) {
            mainCamera = 0;
        } else if (InputManager::instance()->isKeyPressed(GLFW_KEY_2)) {
            mainCamera = 1;
        }

        Camera* selectedCamera = nullptr;

        if (mainCamera == 0) {
            selectedCamera = &camera;
        } else if (mainCamera = 1) {
            selectedCamera = &lightCamera;
        }

        const float MovementSpeed =  2.5f;
        float velocity = MovementSpeed * dt / 1000.0;

        if (InputManager::instance()->isKeyPressed(GLFW_KEY_E)) {
            selectedCamera->m_Rotation.x -= (float)InputManager::instance()->mouseMoveX * 0.0045f;
            selectedCamera->m_Rotation.y += (float)InputManager::instance()->mouseMoveY * 0.0045f;
        }

        if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_W)) {
            selectedCamera->m_Position += selectedCamera->forwardVec * velocity;
        } else if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_S)) {
            selectedCamera->m_Position -= selectedCamera->forwardVec * velocity;
        }

        if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_A)) {
            selectedCamera->m_Position += selectedCamera->rightVec * velocity;
        } else if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_D)) {
            selectedCamera->m_Position -= selectedCamera->rightVec * velocity;
        }   

        camera.update();
        lightCamera.update();
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