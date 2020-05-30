#pragma once

#include "Input/InputManager.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Graphics/ShaderPipeline.h"
#include "Graphics/ShaderInputLayout.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/ShaderBuffer.h"
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include <entt/entt.hpp>
#include "Components.h"
#include "Graphics/Texture.h"
#include "Graphics/vertex.h"
#include "Camera.h"
#include "Model/Model.h"


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

public:
    mvp projectionData;
    ModelData modelData;
    MeshData meshData;

    std::shared_ptr<ShaderPipeline> shaderPipeline;
    std::shared_ptr<ShaderInputLayout> shaderInputLayout;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<ShaderBuffer> shaderBuffer;
    std::shared_ptr<ShaderBuffer> modelShaderBuffer;
    std::shared_ptr<ShaderBuffer> meshShaderBuffer;

    entt::registry* registry;
    
    float cameraX = -1.5f;
    float cameraY = -1.5f;

    double cameraRotX = 0.0f;
    double cameraRotY = 0.0f;

    Camera camera;
public:
	void init(entt::registry* registry) {
        this->registry = registry;

        const char* vertexShaderCode = 
            "#version 460\n"
            "layout(location = 0) in vec3 pos;\n"
            "layout(location = 1) in vec2 texCoords;\n"
            "layout(location = 2) in vec3 normal;\n"
            "layout(std140, binding = 0) uniform Matrices\n"
            "{\n"
            "    mat4 WVP;\n"
            "};\n"
            "layout(std140, binding = 1) uniform Model\n"
            "{\n"
            "    mat4 toWorld;\n"
            "    mat4 inverseToWorld;\n"
            "};\n"
            "layout(std140, binding = 2) uniform Mesh\n"
            "{\n"
            "    mat4 transform;\n"
            "    mat4 matrixTransform;\n"
            "};\n"
            "out VS_OUT\n"
            "{\n"
            "    vec4 position;\n"
            "    vec2 texCoord;\n"
            "    vec3 normal;\n"
            "} vs_out;\n"
            "out gl_PerVertex\n"
            "{\n"
            "  vec4 gl_Position;\n"
            "  float gl_PointSize;\n"
            "  float gl_ClipDistance[];\n"
            "};\n"
            "\n"
            "void main() {\n"
            "    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0f) * transform * toWorld * WVP;\n"
            "    vs_out.normal = normalize(vec3(inverseToWorld * matrixTransform * vec4(normalize(normal), 0.0f)));\n"
            "    vs_out.texCoord = texCoords;\n"
            "    return;\n"
            "}\n";

        const char* pixelShaderCode = 
            "#version 460\n"
            "out vec4 FragColor;\n"
            "in VS_OUT\n"
            "{\n"
            "    vec4 position;\n"
            "    vec2 texCoord;\n"
            "    vec3 normal;\n"
            "} fs_in;\n"
            "uniform sampler2D diffuseTexture; //0\n"
            "uniform sampler2D normalTexture;  //1\n"
            "void main()\n"
            "{\n"
            "    vec4 color = texture(diffuseTexture, fs_in.texCoord);\n"
            "    FragColor = vec4(0.4f, 0.4f, 0.4f, 1.0f) + clamp(dot(vec3(0.0f, 1.0f, 0.0f), fs_in.normal), 0.0f, 1.0f) * vec4(0.3f, 0.3f, 0.3f, 1.0f);"
            "    FragColor.rgb = fs_in.normal;"
            "}\n";

        shaderPipeline = std::make_shared<ShaderPipeline>(vertexShaderCode, pixelShaderCode);
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

        vertexBuffer = std::make_shared<VertexBuffer>(data.size(), sizeof(vertex), data.data());

        shaderBuffer = std::make_shared<ShaderBuffer>(sizeof(projectionData), &projectionData);
        modelShaderBuffer = std::make_shared<ShaderBuffer>(sizeof(modelData), &modelData);
        meshShaderBuffer = std::make_shared<ShaderBuffer>(sizeof(meshData), &meshData);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        camera.setProjectionMatrix(80.0f, 16.0f / 9.0f, 0.2f, 100000.f);
        camera.m_Position.z = -15.0f;

        glEnable(GL_DEPTH_TEST);
	}

	void update(double dt) {
		shaderPipeline->bind();
        shaderInputLayout->bind();

		projectionData.projection = glm::transpose(camera.getPerspectiveMatrix() * camera.getViewMatrix());
		shaderBuffer->update(&projectionData);
		shaderBuffer->bind(0);

        registry->view<Transform, Render>().each([&](Transform& transform, Render& render) {
            modelData.toWorld = glm::mat4(1.0f);
            modelData.toWorld = glm::translate(modelData.toWorld, transform.translation);
            modelData.toWorld = modelData.toWorld * glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
            modelData.toWorld = glm::scale(modelData.toWorld, transform.scale);
            modelData.inverseToWorld = glm::inverse(modelData.toWorld);

            modelShaderBuffer->update(&modelData);
            modelShaderBuffer->bind(1);

            this->updateTransform(render.model->getRootNode(), render.model.get(), glm::mat4(1.0f));

            const std::vector<std::shared_ptr<Model::SubMesh>>& submeshes = render.model->getSubMeshes();
            for (auto& node : render.model->getNodes()) {
                if (node->mesh == -1) continue;

                const std::shared_ptr<Model::SubMesh>& submesh = submeshes[node->mesh];
                meshData.transform = glm::transpose(node->transform.worldTransform);
                meshData.matrixTransform = glm::transpose(node->transform.matrixTransform);
                meshShaderBuffer->update(&meshData);
                meshShaderBuffer->bind(2);

                submesh->vBuffer->bind();
                submesh->iBuffer->bind();

                glDrawElements(GL_TRIANGLES, submesh->iBuffer->getSize(), GL_UNSIGNED_INT, 0);
            }
        });

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