#pragma once

#include <btBulletDynamicsCommon.h>
#include "Graphics/Physics/PhysicsDebugDraw.h"
#include <BulletCollision\CollisionDispatch\btCollisionWorld.h>
#include <BulletCollision\CollisionDispatch\btGhostObject.h>

class PhysicsSystem {
private:
	std::shared_ptr<btSequentialImpulseConstraintSolver> solver;
	std::shared_ptr<btBroadphaseInterface> overlappingPairCache;
	std::shared_ptr<btCollisionDispatcher> dispatcher;
	std::shared_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	entt::registry* registry;
public:
	std::shared_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
	bool enabled = true;
public:
	PhysicsSystem(entt::registry* registry) :
		registry(registry)
	{
		collisionConfiguration = std::make_shared<btDefaultCollisionConfiguration>();
		dispatcher = std::make_shared<btCollisionDispatcher>(collisionConfiguration.get());
		overlappingPairCache = std::make_shared<btDbvtBroadphase>();
		solver = std::make_shared<btSequentialImpulseConstraintSolver>();

		dynamicsWorld = std::make_shared<btDiscreteDynamicsWorld>(dispatcher.get(), overlappingPairCache.get(), solver.get(), collisionConfiguration.get());
		dynamicsWorld->setGravity(btVector3(0, -10.0f, 0));
		dynamicsWorld->setDebugDrawer(new PhysicsDebugDraw());
	}

	PhysicsComponent& createPhysicsBody(const entt::entity& entity, btCollisionShape* shape, float mass = 0.0f, glm::vec3 originalPos = glm::vec3(0.0), bool isDynamic = false) {
		btTransform startTransform;
		startTransform.setIdentity();

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);

		startTransform.setOrigin(btVector3(originalPos.x, originalPos.y, originalPos.z));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
		
		btRigidBody* body = new btRigidBody(rbInfo);
		//body->setFriction(-1.0f);
		//body->setRollingFriction(-1.0f);
		//body->setActivationState(DISABLE_DEACTIVATION);
		dynamicsWorld->addRigidBody(body);

		PhysicsComponent& physicsComponent = registry->emplace<PhysicsComponent>(entity, shape, motionState, body);
		physicsComponent.isDynamic = isDynamic;

		return physicsComponent;
	}

	void update(double dt) {
#if 0
		registry->view<TransformComponent, PhysicsComponent>().each([&](TransformComponent& transform, PhysicsComponent& physics) {
			if (physics.body == nullptr) return;

			btTransform physicsTransform;
			physicsTransform.setIdentity();

			physicsTransform.setOrigin(btVector3(transform.translation.x, transform.translation.y, transform.translation.z));
			physicsTransform.setRotation(physics.body->getWorldTransform().getRotation());
			physics.body->setWorldTransform(physicsTransform);
			physics.body->activate(true);
		});
#endif
		if (enabled) {
			dynamicsWorld->stepSimulation(dt / 1000.0);
		}

		//int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
		//for (int i = 0; i < numManifolds; i++) {
		//	btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		//	const btCollisionObject* obA = contactManifold->getBody0();
		//	const btCollisionObject* obB = contactManifold->getBody1();

		//	int numContacts = contactManifold->getNumContacts();
		//	for (int j = 0; j < numContacts; j++) {
		//		btManifoldPoint& pt = contactManifold->getContactPoint(j);
		//		if (pt.getDistance() < 0.f) {
		//			const btVector3& ptA = pt.getPositionWorldOnA();
		//			const btVector3& ptB = pt.getPositionWorldOnB();
		//			const btVector3& normalOnB = pt.m_normalWorldOnB;
		//			//printf("collision\n");
		//			bool x = (ContactProcessedCallback)(pt, obA, obB);
		//			if (x)
		//				printf("collision\n");
		//		}
		//	}
		//}

		/*registry->view<TransformComponent, PhysicsComponent>().each([&](TransformComponent& transform, PhysicsComponent& physics) {
			if (physics.body == nullptr) return;

			btTransform tr;

			if (physics.isDynamic) {
				physics.body->getMotionState()->getWorldTransform(tr);
			} else {
				tr = physics.body->getWorldTransform();
			}

			btVector3& pos = tr.getOrigin();

			transform.translation.x = pos.getX();
			transform.translation.y = pos.getY();
			transform.translation.z = pos.getZ();

			btQuaternion rot = tr.getRotation();

			transform.rotationq = glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
			transform.rotation = glm::eulerAngles(transform.rotationq) * 3.14159f / 180.f;

			transform.matrix = glm::translate(glm::mat4(1.0f), transform.translation);
			transform.matrix = transform.matrix * glm::toMat4(transform.rotationq);
			transform.matrix = glm::scale(transform.matrix, transform.scale);
		});*/

		//registry->view<TransformComponent, PlayerPhysics>().each([&](TransformComponent& transform, PlayerPhysics& physics) {
		//	btTransform tr;

		//	physics.body->getMotionState()->getWorldTransform(tr);

		//	btVector3& pos = tr.getOrigin();

		//	transform.translation.x = pos.getX();
		//	transform.translation.y = pos.getY();
		//	transform.translation.z = pos.getZ();

		//	btQuaternion rot = physics.body->getCenterOfMassTransform().getRotation();

		//	if (physics.applyRotation) {
		//		transform.rotationq = glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
		//		transform.rotation = glm::eulerAngles(transform.rotationq) * 3.14159f / 180.f;
		//	}

		//	transform.matrix = glm::translate(glm::mat4(1.0f), transform.translation);
		//	transform.matrix = transform.matrix * glm::toMat4(transform.rotationq);
		//});
	}
};