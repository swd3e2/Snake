#include "OpenGLRenderer.h"

void GLAPIENTRY
MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::cout << message << std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_RELEASE) {
		InputManager::instance()->setKeyPressed(key, false);
	}
	else {
		InputManager::instance()->setKeyPressed(key, true);
	}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	InputManager::instance()->setCursorPosition(xpos, ypos);
}