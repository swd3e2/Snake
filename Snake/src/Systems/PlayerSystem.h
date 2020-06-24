#pragma once

#include "Components.h"
#include <entt/entt.hpp>
#include "Input/InputManager.h"
#include <btBulletDynamicsCommon.h>

class PlayerSystem {
private:
	entt::registry* registry;
public:
	PlayerSystem(entt::registry* registry) : registry(registry) {}

	void update(double dt) {
		registry->view<Physics, CameraComponent, Transform, PlayerComponent>().each([&](Physics& physics, CameraComponent& camera, Transform& transform, PlayerComponent& player) {
			if (!player.active) return;

			/*if (InputManager::instance()->isKeyPressed(GLFW_KEY_E)) {
				transform.rotation.x -= (float)InputManager::instance()->mouseMoveX * 0.0045f;
				transform.rotation.y += (float)InputManager::instance()->mouseMoveY * 0.0045f;
			}

			if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_W)) {
				transform.translation += glm::vec3(camera.forwardVec) * player.speed;
			} else if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_S)) {
				transform.translation -= glm::vec3(camera.forwardVec) * player.speed;
			}

			if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_A)) {
				transform.translation += glm::vec3(camera.rightVec) * player.speed;
			} else if (InputManager::instance()->instance()->isKeyPressed(GLFW_KEY_D)) {
				transform.translation -= glm::vec3(camera.rightVec) * player.speed;
			}*/

			btTransform btTransform;
			btTransform.setIdentity();

			btTransform.setOrigin(btVector3(transform.translation.x, transform.translation.y, transform.translation.z));
			btTransform.setRotation(physics.body->getWorldTransform().getRotation());
			physics.body->setWorldTransform(btTransform);
			physics.body->activate(true);
		});
	}
};