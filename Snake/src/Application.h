#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "RenderSystem.h"
#include "Components.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include "Interface/MainInterface.h"
#include "Model/Import/GltfImporter.h"
#include "Model/Model.h"

void GLAPIENTRY
MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    std::cout << message << std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE) {
        InputManager::instance()->setKeyPressed(key, false);
    } else {
        InputManager::instance()->setKeyPressed(key, true);
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    InputManager::instance()->setCursorPosition(xpos, ypos);
}

class Application {
private:
    entt::registry registry;
    RenderSystem rs;
    GLFWwindow* window;
    std::chrono::time_point<std::chrono::steady_clock> m_Start;

    std::shared_ptr<MainInterface> interface;
public:
    ~Application() {
        glfwTerminate();
    }

	void init() {
        /* Initialize the library */
        if (!glfwInit()) {
            return;
        }

        window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
        if (!window) {
            glfwTerminate();
            return;
        }
        /* Make the window's context current */
        glfwMakeContextCurrent(window);
        glewInit();

        glDebugMessageCallback(MessageCallback, 0);
        glfwSetKeyCallback(window, key_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);

        rs.init(&registry);

        interface = std::make_shared<MainInterface>(window, &registry);

        auto entity = registry.create();
        registry.emplace<Render>(entity, 0, 8);
        registry.emplace<Position>(entity, 10.0f, 10.0f);
        registry.emplace<Player>(entity);
        registry.emplace<Animation>(entity, 0.0, 0, 8, 7, 7);

        registry.create();
        registry.create();

        GltfImporter importer;
        std::shared_ptr<Import::Model> importModel = importer.import("scene.gltf");
        std::shared_ptr<Model> model = std::make_shared<Model>(importModel);

        rs.testModel = model;
        interface->model = model;

        for (auto& it : importModel->meshes[0]->vertices) {
            //std::cout << "x: " << it.normal.x << " y: " << it.normal.y << " z: " << it.normal.z << std::endl;
        }

        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}

	void run() {
        double dt = 0.0;

        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            m_Start = std::chrono::high_resolution_clock::now();
            
            //int lscript = luaL_dofile(L, "src/test.lua");

            rs.update(16.0);
            registry.view<Position, Player>().each([](auto& pos) {
                if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_UP)) {
                    pos.y += 0.1;
                } else if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_DOWN)) {
                    pos.y -= 0.1f;
                }

                if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_LEFT)) {
                    pos.x -= 0.1f;
                } else if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_RIGHT)) {
                    pos.x += 0.1f;
                }
            });


            interface->update(16.0);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            InputManager::instance()->mouseMoveX = 0.0;
            InputManager::instance()->mouseMoveY = 0.0;

            /* Poll for and process events */
            glfwPollEvents();

            dt = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - m_Start).count();
        }
	}
};