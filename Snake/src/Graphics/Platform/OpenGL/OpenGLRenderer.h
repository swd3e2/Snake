#pragma once

#include "Graphics/Renderer.h"
#include <GLFW/glfw3.h>
#include "OpenGLContext.h"
#include "OpenGLMainRenderTarget.h"
#include "Input/InputManager.h"
#include "OpenGLWindow.h"
#include <iostream>

void GLAPIENTRY
MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

class OpenGlRenderer : public Renderer {
private:
	OpenGLWindow* window;
	GLFWwindow* glfwWindow;
public:
    OpenGlRenderer() {
        _rendererType = RendererType::OpenGL;
        _renderContext = new OpenGLContext();
        _mainRenderTarget = new OpenGLMainRenderTarget();
    }

    virtual void drawIndexed(int cnt) {
        glDrawElements(GL_TRIANGLES, cnt, GL_UNSIGNED_INT, 0);
    }

    virtual Window* createWindow(int width, int height) override {
		/* Initialize the library */
		if (!glfwInit()) {
			return nullptr;
		}

		window = new OpenGLWindow(width, height);
		glfwWindow = window->getWindow();

		/* Make the window's context current */
		glfwMakeContextCurrent(glfwWindow);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "Failed to initialize GLAD" << std::endl;
		}

		glDebugMessageCallback(MessageCallback, 0);
		glfwSetKeyCallback(glfwWindow, key_callback);
		glfwSetCursorPosCallback(glfwWindow, cursor_position_callback);

		return window;
    }

	virtual void swapBuffers() override{
		glfwSwapBuffers(glfwWindow);
	}
};