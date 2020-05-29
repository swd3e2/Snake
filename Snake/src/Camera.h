#pragma once

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

class Camera {
public:
	glm::vec4 m_Position;
	glm::vec4 m_Rotation;

	glm::vec4 forwardVec;
	glm::vec4 rightVec;
private:
	glm::vec4 m_CamTarget;
	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;
	glm::vec4 upVec;
public:
	Camera() {
		m_Position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		m_Rotation = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
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

	void update() {
		glm::mat4 rotation = glm::transpose(glm::eulerAngleYXZ(this->m_Rotation.x, this->m_Rotation.y, 0.0f));

		m_CamTarget = glm::normalize(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) * rotation) + m_Position;
		m_ViewMatrix = glm::lookAt(glm::vec3(m_Position), glm::vec3(m_CamTarget), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 temp = glm::transpose(glm::eulerAngleYXZ(this->m_Rotation.x, this->m_Rotation.y, this->m_Rotation.z));

		forwardVec = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) * temp;
		rightVec = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) * temp;
		upVec = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * temp;
	}
};