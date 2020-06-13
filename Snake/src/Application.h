#pragma once

#include <glad/glad.h>
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
#include "ScriptSystem.h"
#include "Saver.h"
#include "Loader.h"

void GLAPIENTRY
MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
//    std::cout << message << std::endl;
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
    ScriptSystem scriptSystem;
public:
    ~Application() {
        glfwTerminate();
    }

	void init() {
        /* Initialize the library */
        if (!glfwInit()) {
            return;
        }
        
        window = glfwCreateWindow(1920, 1080, "Snake", NULL, NULL);
        if (!window) {
            glfwTerminate();
            return;
        }
        /* Make the window's context current */
        glfwMakeContextCurrent(window);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            // return -1;
        }

        //glfwSwapInterval( 0 );

        glDebugMessageCallback(MessageCallback, 0);
        glfwSetKeyCallback(window, key_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);

        rs.init(&registry);
        scriptSystem.init(&registry);

        interface = std::make_shared<MainInterface>(window, &registry, &rs.camera, &rs);

        // Saver saver;
        // saver.saveToFile("test.json", &registry);
        Loader loader;
        loader.loadFromFile("test.json", &registry);
	}

	void run() {
        double dt = 0.0;
        
        while (!glfwWindowShouldClose(window)) {
            m_Start = std::chrono::high_resolution_clock::now();

            rs.update(16.0);
            interface->update(dt);
            scriptSystem.update();
            
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