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

class Application {
private:
    entt::registry registry;
    RenderSystem rs;
    GLFWwindow* window;
    std::chrono::time_point<std::chrono::steady_clock> m_Start;
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

        rs.init(&registry);

        std::shared_ptr<Texture> tileTexture = std::make_shared<Texture>("tilemap.png");
        std::shared_ptr<Tilemap> tilemap = std::make_shared<Tilemap>(tileTexture, glm::vec2(8, 8));
        LevelLoader loader;
        auto rawLevel = loader.loadFromFile("tilemap.txt");
        LevelBuilder builder;
        auto level = builder.build(rawLevel, tilemap);
        rs.level = level;


        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);
        glFrontFace(GL_CW);
	}

	void run() {

        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);
            m_Start = std::chrono::high_resolution_clock::now();
            
            //int lscript = luaL_dofile(L, "src/test.lua");

            rs.update(16.0);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();

            auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - m_Start);
            //std::cout << elapsed.count() << std::endl;
        }
	}
};