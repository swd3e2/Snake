#pragma once

#include "ISystem.h"
#include <entt/entt.hpp>
#include "Components.h"
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

class CameraSystem : public ISystem 
{
public:
	CameraSystem(SceneManager* sceneManager) 
		: ISystem(sceneManager)
	{}

	virtual void update(double dt) override
	{
		entt::registry* registry = sceneManager->getCurrentScene()->getRegistry();

		registry->view<CameraComponent, TransformComponent>().each([&](CameraComponent& camera, TransformComponent& transform) {
			glm::mat4 rotation = glm::transpose(glm::eulerAngleYXZ(transform.rotation.x, transform.rotation.y, 0.0f));
			glm::vec3 camTarget = glm::normalize(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) * rotation) + glm::vec4(transform.translation + glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
			camera.viewMatrix = glm::lookAt(transform.translation + glm::vec3(0.0f, 1.0f, 0.0f), camTarget, glm::vec3(0.0f, 1.0f, 0.0f));

			glm::mat4 temp = glm::transpose(glm::eulerAngleYXZ(transform.rotation.x, 0.0f, transform.rotation.z));
			camera.forwardVec = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) * temp;
			camera.rightVec = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) * temp;
		});
	}
};