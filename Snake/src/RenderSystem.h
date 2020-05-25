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
#include "TiledQuad.h"
#include "Graphics/vertex.h"
#include "Game/Level.h"

class RenderSystem {
private:
    struct mvp {
        glm::mat4 projection;
    };
    struct ModelData {
        glm::mat4 toWorld;
    };

public:
    mvp projectionData;
    ModelData modelData;

    std::shared_ptr<ShaderPipeline> shaderPipeline;
    std::shared_ptr<ShaderInputLayout> shaderInputLayout;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<ShaderBuffer> shaderBuffer;
    std::shared_ptr<ShaderBuffer> modelShaderBuffer;
    std::shared_ptr<Level> level;

    entt::registry* registry;
    std::shared_ptr<Texture> texture;
    TiledQuad* map[15][8];
    
    float cameraX = -1.5f;
    float cameraY = -1.5f;
public:
	void init(entt::registry* registry) {
        this->registry = registry;

        const char* vertexShaderCode = 
            "#version 460\n"
            "layout(location = 0) in vec4 pos;\n"
            "layout(location = 1) in vec2 texCoords;\n"
            "layout(location = 2) in vec2 test;\n"
            "layout(std140, binding = 0) uniform Matrices\n"
            "{\n"
            "    mat4 WVP;\n"
            "};\n"
            "layout(std140, binding = 1) uniform Model\n"
            "{\n"
            "    mat4 toWorld;\n"
            "};\n"
            "out VS_OUT\n"
            "{\n"
            "    vec4 position;\n"
            "    vec2 texCoord;\n"
            "} vs_out;\n"
            "out gl_PerVertex\n"
            "{\n"
            "  vec4 gl_Position;\n"
            "  float gl_PointSize;\n"
            "  float gl_ClipDistance[];\n"
            "};\n"
            "\n"
            "void main() {\n"
            "    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0f) * toWorld * WVP;\n"
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
            "} fs_in;\n"
            "uniform sampler2D diffuseTexture; //0\n"
            "void main()\n"
            "{\n"
            "    vec4 color = texture(diffuseTexture, fs_in.texCoord);\n"
            "    if (color.r == 1 && color.b == 1) discard;"
            "    FragColor = color;"
            "}\n";

        shaderPipeline = std::make_shared<ShaderPipeline>(vertexShaderCode, pixelShaderCode);
        shaderInputLayout = std::make_shared<ShaderInputLayout>();

        shaderInputLayout->add({ InputDataType::Float4, "pos" });
        shaderInputLayout->add({ InputDataType::Float2, "texCoords" });
        shaderInputLayout->add({ InputDataType::Float2, "test" });

        std::vector<vertex> data;
        data.push_back(vertex(-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f));
        data.push_back(vertex(0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f));
        data.push_back(vertex(0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f));

        vertexBuffer = std::make_shared<VertexBuffer>(data.size(), sizeof(vertex), data.data());

        shaderBuffer = std::make_shared<ShaderBuffer>(sizeof(projectionData), &projectionData);
        modelShaderBuffer = std::make_shared<ShaderBuffer>(sizeof(modelData), &modelData);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        texture = std::make_shared<Texture>("tilemap.png");

        for (int y = 0; y < 15; y++) {
            for (int x = 0; x < 8; x++) {
                map[14 - y][x] = new TiledQuad(texture, { x, y }, { 8, 8 });
            }
        }
	}

	void update(double dt) {
		shaderPipeline->bind();
        shaderInputLayout->bind();

		projectionData.projection = glm::transpose(glm::perspective(glm::radians(80.0f), 16.0f / 9.0f, 0.2f, 100.f) 
            * glm::translate<float>(glm::mat4(1.0f), glm::vec3(cameraX, cameraY, -15.0f)));
		shaderBuffer->update(&projectionData);
		shaderBuffer->bind(0);

        level->vertexBuffer->bind();
        level->indexBuffer->bind();
        level->texture->bind(0);

        modelData.toWorld = glm::mat4(1.0f);
        modelShaderBuffer->update(&modelData);
        modelShaderBuffer->bind(1);

        glDrawElements(GL_TRIANGLES, level->indexBuffer->getSize(), GL_UNSIGNED_INT, 0);

        auto view = registry->view<Position, Render>();
        for (auto entity : view) {
            Position& position = view.get<Position>(entity);
            Render& render = view.get<Render>(entity);

            modelData.toWorld = glm::transpose(glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f)));
            modelShaderBuffer->update(&modelData);
            modelShaderBuffer->bind(1);

            TiledQuad* quad = this->map[render.yIndex][render.xIndex];
            quad->vertexBuffer->bind();
            quad->indexBuffer->bind();

            glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);
        }

        if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_W)) {
            cameraY -= 0.2;
        } else if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_S)) {
            cameraY += 0.2f;
        }

        if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_A)) {
            cameraX += 0.2f;
        } else if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_D)) {
            cameraX -= 0.2f;
        }
	}
};