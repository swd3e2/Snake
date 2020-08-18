#pragma once

#include "Graphics/Camera.h"
#include "Input/InputManager.h"

enum class DebugCameraMode {
	RPG,
	FPS
};

class DebugCameraController {
private:
	DebugCameraMode mode;
public:
	DebugCameraController() : 
		mode(DebugCameraMode::RPG)
	{}

	void update(Camera* camera, double dt) 
	{
		switch (mode)
		{
		case DebugCameraMode::RPG:
			updateRpgCamera(camera, dt);
			break;
		case DebugCameraMode::FPS:
			updateFpsCamera(camera, dt);
			break;
		}
	}

    void updateRpgCamera(Camera* camera, double dt)
    {
		if (InputManager::instance()->instance()->isKeyPressed(87)) {
			camera->Position -= glm::vec3(1.0f, 0.0, 0.0) * 4.0f * (float)dt / 1000.0f;
		} else if (InputManager::instance()->instance()->isKeyPressed(83)) {
			camera->Position += glm::vec3(1.0f, 0.0, 0.0) * 4.0f * (float)dt / 1000.0f;
		}

		if (InputManager::instance()->instance()->isKeyPressed(65)) {
			camera->Position += glm::vec3(0.0f, 0.0, 1.0) * 4.0f * (float)dt / 1000.0f;
		} else if (InputManager::instance()->instance()->isKeyPressed(68)) {
			camera->Position -= glm::vec3(0.0f, 0.0, 1.0) * 4.0f * (float)dt / 1000.0f;
		}
    }

    void updateFpsCamera(Camera* camera, double dt)
    {
		if (InputManager::instance()->instance()->isKeyPressed(69)) {
			double d = 1 - exp(log(0.5) * 130 * (dt * 0.00001));
			float MouseSensitivity = 1.1f;
			float xoffset = (float)InputManager::instance()->mouseMoveX * MouseSensitivity;
			float yoffset = (float)InputManager::instance()->mouseMoveY * MouseSensitivity;
			camera->rotateCamera(xoffset, yoffset);
		}


		if (InputManager::instance()->instance()->isKeyPressed(87)) {
			camera->Position -= camera->Front * 4.0f * (float)dt / 1000.0f;
		} else if (InputManager::instance()->instance()->isKeyPressed(83)) {
			camera->Position += camera->Front * 4.0f * (float)dt / 1000.0f;
		}

		if (InputManager::instance()->instance()->isKeyPressed(65)) {
			camera->Position += camera->Right * 4.0f * (float)dt / 1000.0f;
		} else if (InputManager::instance()->instance()->isKeyPressed(68)) {
			camera->Position -= camera->Right * 4.0f * (float)dt / 1000.0f;
		}
    }
};