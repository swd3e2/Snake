#pragma once

#include "Input/InputManager.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Graphics/Renderer/ShaderPipeline.h"
#include "Graphics/Renderer/ShaderInputLayout.h"
#include "Graphics/Renderer/VertexBuffer.h"
#include "Graphics/Renderer/ConstantBuffer.h"
#include "Graphics/Renderer/Texture.h"
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
public:
    mvp projectionData;
    ModelData modelData;
    MeshData meshData;
    MaterialData materialData;
    OpenGlRenderer renderer;
    
    std::shared_ptr<ShaderPipeline> shaderPipeline;
    std::shared_ptr<ShaderInputLayout> shaderInputLayout;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<ConstantBuffer> shaderBuffer;
    std::shared_ptr<ConstantBuffer> modelShaderBuffer;
    std::shared_ptr<ConstantBuffer> meshShaderBuffer;
    std::shared_ptr<ConstantBuffer> materialShaderBuffer;

    entt::registry* registry;
    
    float cameraX = -1.5f;
    float cameraY = -1.5f;

    double cameraRotX = 0.0f;
    double cameraRotY = 0.0f;

    Camera camera;
public:
	void init(entt::registry* registry) {
        this->registry = registry;

        File fileVS("E:/dev/Snake/Debug/Debug/defaultVS.glsl");
        File filePS("E:/dev/Snake/Debug/Debug/defaultPS.glsl");

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

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        camera.setProjectionMatrix(80.0f, 16.0f / 9.0f, 0.2f, 100000.f);
        camera.m_Position.z = -15.0f;

        glEnable(GL_DEPTH_TEST);
	}

	void update(double dt) {
        renderer.bindShaderPipeline(shaderPipeline);
        shaderInputLayout->bind();

		projectionData.projection = glm::transpose(camera.getPerspectiveMatrix() * camera.getViewMatrix());
		shaderBuffer->update(&projectionData);
		renderer.bindConstantBuffer(shaderBuffer);

        registry->view<Transform, Render>().each([&](Transform& transform, Render& render) {
            modelData.toWorld = glm::mat4(1.0f);
            modelData.toWorld = glm::translate(modelData.toWorld, transform.translation);
            modelData.toWorld = modelData.toWorld * glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
            modelData.toWorld = glm::transpose(glm::scale(modelData.toWorld, transform.scale));
            modelData.inverseToWorld = glm::inverse(modelData.toWorld);

            modelShaderBuffer->update(&modelData);
		    renderer.bindConstantBuffer(modelShaderBuffer);

            materialData.color = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
            materialShaderBuffer->update(&materialData);
            renderer.bindConstantBuffer(materialShaderBuffer);

            this->updateTransform(render.model->getRootNode(), render.model.get(), glm::mat4(1.0f));

            const std::vector<std::shared_ptr<Model::SubMesh>>& submeshes = render.model->getSubMeshes();
            for (auto& node : render.model->getNodes()) {
                if (node->mesh == -1) continue;

                const std::shared_ptr<Model::SubMesh>& submesh = submeshes[node->mesh];
                meshData.transform = glm::transpose(node->transform.worldTransform);
                meshData.matrixTransform = glm::transpose(node->transform.matrixTransform);
                meshShaderBuffer->update(&meshData);
                renderer.bindConstantBuffer(meshShaderBuffer);

                renderer.bindVertexBuffer(submesh->vBuffer);
                renderer.bindIndexBuffer(submesh->iBuffer);

                renderer.drawIndexed(submesh->iBuffer->getSize());
            }
        });

        while(!commands.empty()) {
            std::function<void()> command = commands.back();
            command();
            commands.pop();
        }

        if (InputManager::instance()->isKeyPressed(GLFW_KEY_E)) {
            camera.m_Rotation.x -= (float)InputManager::instance()->mouseMoveX * 0.0045f;
            camera.m_Rotation.y += (float)InputManager::instance()->mouseMoveY * 0.0045f;
        }

        if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_W)) {
            camera.m_Position += camera.forwardVec * 11.2f;
        } else if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_S)) {
            camera.m_Position -= camera.forwardVec * 11.2f;
        }

        if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_A)) {
            camera.m_Position += camera.rightVec * 11.2f;
        } else if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_D)) {
            camera.m_Position -= camera.rightVec * 11.2f;
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