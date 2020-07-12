#pragma once

#include "Components.h"
#include <entt/entt.hpp>
#include "Input/InputManager.h"
#include <btBulletDynamicsCommon.h>


class IgnoreBodyAndGhostCast : public btCollisionWorld::ClosestRayResultCallback
{
private:
	btRigidBody* m_pBody;
	btPairCachingGhostObject* m_pGhostObject;
public:
	IgnoreBodyAndGhostCast(btRigidBody* pBody, btPairCachingGhostObject* pGhostObject)
		: btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0)),
		m_pBody(pBody), m_pGhostObject(pGhostObject)
	{}

	btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
	{
		if (rayResult.m_collisionObject == m_pBody || rayResult.m_collisionObject == m_pGhostObject)
			return 1.0f;

		return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
	}
};

class PlayerSystem {
private:
	entt::registry* registry;
	PhysicsSystem* physicsSystem;
public:
	PlayerSystem(entt::registry* registry, PhysicsSystem* physicsSystem) : registry(registry), physicsSystem(physicsSystem) {}

	void update(double dt) {
		registry->view<PhysicsComponent, CameraComponent, TransformComponent, PlayerComponent>().each([&](PhysicsComponent& physics, CameraComponent& camera, TransformComponent& transform, PlayerComponent& player) {
			if (!player.active) return;

			if (InputManager::instance()->isKeyPressed(69)) {
				transform.rotation.x -= (float)InputManager::instance()->mouseMoveX * 2.0 * dt / 1000.0;
				transform.rotation.y += (float)InputManager::instance()->mouseMoveY * 2.0 * dt / 1000.0;
			}

			glm::vec3 velocity = glm::vec3(0.0f);

			if (InputManager::instance()->instance()->isKeyPressed(87)) {
				velocity = glm::vec3(camera.forwardVec) * player.speed;// *(float)dt;
			} else if (InputManager::instance()->instance()->isKeyPressed(83)) {
				velocity = -glm::vec3(camera.forwardVec) * player.speed;// * (float)dt;
			}

			if (InputManager::instance()->instance()->isKeyPressed(65)) {
				velocity += glm::vec3(camera.rightVec) * player.speed;// * (float)dt;
			} else if (InputManager::instance()->instance()->isKeyPressed(68)) {
				velocity += -glm::vec3(camera.rightVec) * player.speed;// * (float)dt;
			}

			// Ray cast, ignore rigid body
			btVector3 from = physics.body->getWorldTransform().getOrigin();
			btVector3 to = physics.body->getWorldTransform().getOrigin() - btVector3(0.0f, 1.6f, 0.0f);

			//btVector3 from = btVector3(1000.0, 100000.0, 10.0);
			//btVector3 to = from - btVector3(0.0f, 1.0f, 0.0f);
			/*std::cout << "from x " << from.x() << " y " << from.y() << " z " << from.z() << std::endl;
			std::cout << "to x " << to.x() << " y " << to.y() << " z " << to.z() << std::endl;*/

			btCollisionWorld::ClosestRayResultCallback res(from, to);
			physicsSystem->dynamicsWorld->rayTest(from, to, res);

			physics.m_onGround = false;
			if (res.hasHit()) {
				physics.m_onGround = true;
			}

			//std::cout << (physics.m_onGround ? "ON GROUND" : "NOT ON GROUND") << std::endl;

			if (InputManager::instance()->instance()->isKeyPressed(32) && physics.m_onGround) {
				physics.body->applyCentralImpulse(btVector3(0.0f, 70.0f, 0.0f));
				// Move upwards slightly so velocity isn't immediately canceled when it detects it as on ground next frame
				const float jumpYOffset = 0.01f;
				float previousY = physics.body->getWorldTransform().getOrigin().getY();
				//physics.body->getWorldTransform().getOrigin().setY(previousY + jumpYOffset);
			}

			glm::vec2 velocityXZ(velocity.x, velocity.z);
			// Prevent from going over maximum speed
			float speedXZ = glm::length(velocityXZ);

			float m_maxSpeed = 12.0f;
			if (speedXZ > m_maxSpeed)
				velocityXZ = velocityXZ / speedXZ * m_maxSpeed;

			player.m_manualVelocity.x = velocityXZ.x;
			player.m_manualVelocity.z = velocityXZ.y;
			//std::cout << "x " << player.m_manualVelocity.x << " y " << player.m_manualVelocity.y << std::endl;

			player.m_manualVelocity.y = physics.body->getLinearVelocity().getY();

			physics.body->setLinearVelocity(btVector3(player.m_manualVelocity.x, player.m_manualVelocity.y, player.m_manualVelocity.z));
			//physics.m_pGhostObject->setWorldTransform(physics.body->getWorldTransform());

			//btTransform btTransform;
			//btTransform.setIdentity();

			//btTransform.setOrigin(btVector3(transform.translation.x, transform.translation.y, transform.translation.z));
			//btTransform.setRotation(physics.body->getWorldTransform().getRotation());
			//physics.body->setWorldTransform(btTransform);
			//physics.body->activate(true);
		});
	}
};