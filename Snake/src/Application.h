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
#include "Game/Loader/LevelLoader.h"
#include "Game/LevelBuilder.h"
#include "Tilemap.h"
#include "Interface/MainInterface.h"

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

        window = glfwCreateWindow(1366, 768, "Hello World", NULL, NULL);
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

        std::shared_ptr<Texture> tileTexture = std::make_shared<Texture>("tilemap.png");
        std::shared_ptr<Tilemap> tilemap = std::make_shared<Tilemap>(tileTexture, glm::vec2(8, 8));
        LevelLoader loader;
        auto rawLevel = loader.loadFromFile("tilemap.txt");
        LevelBuilder builder;
        auto level = builder.build(rawLevel, tilemap);
        rs.level = level;

        interface = std::make_shared<MainInterface>(window, &registry);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);
        glFrontFace(GL_CW);

        auto entity = registry.create();
        registry.emplace<Render>(entity, 0, 8);
        registry.emplace<Position>(entity, 10.0f, 10.0f);
        registry.emplace<Player>(entity);
        registry.emplace<Animation>(entity, 0.0, 0, 8, 7, 7);

        registry.create();
        registry.create();
	}

	void run() {
        double dt = 0.0;

        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);
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


            registry.view<Animation, Render>().each([dt](auto& animation, auto& render) {
                animation.animationTime += dt;

                if (animation.animationTime > 300.0) {
                    animation.animationTime = 0.0;

                    if (render.xIndex == animation.startX && render.yIndex == animation.startY) {
                        render.xIndex = animation.endX;
                        render.yIndex = animation.endY;
                    } else {
                        render.xIndex = animation.startX;
                        render.yIndex = animation.startY;
                    }
                }
            });
            interface->update(16.0);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();

            dt = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - m_Start).count();

            //std::cout << elapsed.count() << std::endl;
        }
	}
};