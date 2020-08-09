#pragma once

#include "Components.h"
#include <entt/entt.hpp>
#include "Input/InputManager.h"
#include <btBulletDynamicsCommon.h>
#include "Graphics/Camera.h"
#include "EventSystem/EventSystem.h"

class PlayerSystem {
public:
	Camera* mainCamera;
	entt::entity player;
private:
	entt::registry* registry;
	PhysicsSystem* physicsSystem;
	EventSystem* eventSystem;
public:
	PlayerSystem(EventSystem* eventSystem, entt::registry* registry, PhysicsSystem* physicsSystem) : registry(registry), physicsSystem(physicsSystem), eventSystem(eventSystem) {
		ClassEventDelegate<PlayerSystem>* eventDeleage = new ClassEventDelegate<PlayerSystem>(this, &PlayerSystem::onMouseClick);
		eventSystem->addEventListener<PlayerSystem, LeftMouseClickEvent>(eventDeleage);
	}

	void update(double dt) 
	{
		registry->view<TransformComponent, PlayerComponent>().each([&](TransformComponent& transform, PlayerComponent& player) {
			//if (!player.active) return;

			//if (InputManager::instance()->isKeyPressed(69)) {
			//	transform.rotation.x -= (float)InputManager::instance()->mouseMoveX / 2.5;
			//	transform.rotation.y += (float)InputManager::instance()->mouseMoveY / 2.5;
			//}

			//glm::vec3 velocity = glm::vec3(0.0f);

			//if (InputManager::instance()->instance()->isKeyPressed(87)) {
			//	velocity = glm::vec3(camera.forwardVec) * player.speed;// *(float)dt;
			//} else if (InputManager::instance()->instance()->isKeyPressed(83)) {
			//	velocity = -glm::vec3(camera.forwardVec) * player.speed;// * (float)dt;
			//}

			//if (InputManager::instance()->instance()->isKeyPressed(65)) {
			//	velocity += glm::vec3(camera.rightVec) * player.speed;// * (float)dt;
			//} else if (InputManager::instance()->instance()->isKeyPressed(68)) {
			//	velocity += -glm::vec3(camera.rightVec) * player.speed;// * (float)dt;
			//}

			//// Ray cast, ignore rigid body
			//btVector3 from = physics.body->getWorldTransform().getOrigin();
			//btVector3 to = physics.body->getWorldTransform().getOrigin() - btVector3(0.0f, 1.6f, 0.0f);

			////btVector3 from = btVector3(1000.0, 100000.0, 10.0);
			////btVector3 to = from - btVector3(0.0f, 1.0f, 0.0f);
			///*std::cout << "from x " << from.x() << " y " << from.y() << " z " << from.z() << std::endl;
			//std::cout << "to x " << to.x() << " y " << to.y() << " z " << to.z() << std::endl;*/

			//btCollisionWorld::ClosestRayResultCallback res(from, to);
			//physicsSystem->dynamicsWorld->rayTest(from, to, res);

			//physics.m_onGround = false;
			//if (res.hasHit()) {
			//	physics.m_onGround = true;
			//}

			////std::cout << (physics.m_onGround ? "ON GROUND" : "NOT ON GROUND") << std::endl;

			//if (InputManager::instance()->instance()->isKeyPressed(32) && physics.m_onGround) {
			//	physics.body->applyCentralImpulse(btVector3(0.0f, 70.0f, 0.0f));
			//	// Move upwards slightly so velocity isn't immediately canceled when it detects it as on ground next frame
			//	const float jumpYOffset = 0.01f;
			//	float previousY = physics.body->getWorldTransform().getOrigin().getY();
			//	//physics.body->getWorldTransform().getOrigin().setY(previousY + jumpYOffset);
			//}

			//glm::vec2 velocityXZ(velocity.x, velocity.z);
			//// Prevent from going over maximum speed
			//float speedXZ = glm::length(velocityXZ);

			//float m_maxSpeed = 12.0f;
			//if (speedXZ > m_maxSpeed)
			//	velocityXZ = velocityXZ / speedXZ * m_maxSpeed;

			//player.m_manualVelocity.x = velocityXZ.x;
			//player.m_manualVelocity.z = velocityXZ.y;
			////std::cout << "x " << player.m_manualVelocity.x << " y " << player.m_manualVelocity.y << std::endl;

			//player.m_manualVelocity.y = physics.body->getLinearVelocity().getY();

			//physics.body->setLinearVelocity(btVector3(player.m_manualVelocity.x, player.m_manualVelocity.y, player.m_manualVelocity.z));
			////physics.m_pGhostObject->setWorldTransform(physics.body->getWorldTransform());

			////btTransform btTransform;
			////btTransform.setIdentity();

			////btTransform.setOrigin(btVector3(transform.translation.x, transform.translation.y, transform.translation.z));
			////btTransform.setRotation(physics.body->getWorldTransform().getRotation());
			////physics.body->setWorldTransform(btTransform);
			////physics.body->activate(true);
		});
	}

	void onMouseClick(Event* event)
	{
		double x = InputManager::instance()->mousePosX;
		double y = InputManager::instance()->mousePosY;
		glm::mat4x4 viewproj = mainCamera->getPerspectiveMatrix() * mainCamera->getViewMatrix();
		viewproj = glm::transpose(viewproj);
		viewproj = glm::inverse(viewproj);
		glm::vec4 temp = glm::vec4(x / 1920.0 * 2.0 - 1.0, -(y / 1080 * 2.0 - 1.0), 1.0f, 1.0f);
		temp = temp * viewproj;
		temp /= temp.w;
		btVector3 from = btVector3(mainCamera->Position.x, mainCamera->Position.y, mainCamera->Position.z);
		btVector3 to = btVector3(temp.x, temp.y, temp.z);
		btCollisionWorld::ClosestRayResultCallback res(from, to);
		physicsSystem->dynamicsWorld->rayTest(from, to, res);

		TransformComponent& transform = registry->get<TransformComponent>(player);
		if (res.hasHit()) {
			if (registry->has<MoveComponent>(player)) {
				registry->remove<MoveComponent>(player);
			}
			//registry->emplace<MoveComponent>(player, glm::vec3(res.m_hitPointWorld.x(), res.m_hitPointWorld.y() + 0.5f, res.m_hitPointWorld.z()));
		}
	}
};