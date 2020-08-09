#pragma once

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

class Camera {
public:
	glm::vec3 Position;

	glm::vec4 forwardVec;
	glm::vec4 rightVec;

	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// euler Angles
	float Yaw = 0.0f;
	float Pitch = 0.0f;
	float MouseSensitivity = 0.05f;
private:
	glm::vec4 m_CamTarget;
	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;
public:
	Camera() {
		Position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	}

	glm::mat4 getViewMatrix() {
		return m_ViewMatrix;
	}

	glm::mat4 getPerspectiveMatrix() {
		return m_ProjectionMatrix;
	}

	void setProjectionMatrix(float fov, float aspectRatio, float nearZ, float farZ) {
		m_ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearZ, farZ);
	}

	void setOrthographicMatrix(float left, float right, float bottom, float top, float near, float far) {
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, near, far);
	}

	void rotateCamera(float xoffset, float yoffset)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;

		update();
	}

	void update() {
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));

		m_ViewMatrix = glm::lookAtLH(Position, Position + Front, Up);
	}
};