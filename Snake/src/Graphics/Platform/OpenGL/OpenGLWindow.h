#pragma once

#include "Window.h"

class OpenGLWindow : public Window {
private:
	GLFWwindow* window;
public:
	OpenGLWindow(int width, int height) :
		Window(width, height)
	{
		window = glfwCreateWindow(width, height, "Snake", NULL, NULL);
		if (!window) {
			glfwTerminate();
			return;
		}
	}

	virtual bool isOpen() override {
		return glfwWindowShouldClose(window);
	}

	virtual void pollEvents() override {
		/* Poll for and process events */
		glfwPollEvents();
	}

	GLFWwindow* getWindow() { return window; }
};