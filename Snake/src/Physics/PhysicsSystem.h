#pragma once

#include <btBulletDynamicsCommon.h>
#include "PhysicsDebugDraw.h"

class PhysicsSystem {
private:
	std::shared_ptr<btSequentialImpulseConstraintSolver> solver;
	std::shared_ptr<btBroadphaseInterface> overlappingPairCache;
	std::shared_ptr<btCollisionDispatcher> dispatcher;
	std::shared_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	entt::registry* registry;
public:
	std::shared_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
public:
	PhysicsSystem(entt::registry* registry) :
		registry(registry)
	{
		collisionConfiguration = std::make_shared<btDefaultCollisionConfiguration>();
		///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		dispatcher = std::make_shared<btCollisionDispatcher>(collisionConfiguration.get());
		///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		overlappingPairCache = std::make_shared<btDbvtBroadphase>();
		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		solver = std::make_shared<btSequentialImpulseConstraintSolver>();

		dynamicsWorld = std::make_shared<btDiscreteDynamicsWorld>(dispatcher.get(), overlappingPairCache.get(), solver.get(), collisionConfiguration.get());
		dynamicsWorld->setGravity(btVector3(0, -10.0f, 0));
		dynamicsWorld->setDebugDrawer(new PhysicsDebugDraw());
	}

	void createPhysicsBody(const entt::entity& entity, std::shared_ptr<btCollisionShape> shape, float mass = 0.0f, glm::vec3 originalPos = glm::vec3(0.0), bool isDynamic = false) {
		btTransform startTransform;
		startTransform.setIdentity();

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);

		startTransform.setOrigin(btVector3(originalPos.x, originalPos.y, originalPos.z));
		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		std::shared_ptr<btDefaultMotionState>myMotionState = std::make_shared<btDefaultMotionState>(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState.get(), shape.get(), localInertia);

		std::shared_ptr<btRigidBody> body = std::make_shared<btRigidBody>(rbInfo);
		dynamicsWorld->addRigidBody(body.get());

		Physics& physicsComponent = registry->emplace<Physics>(entity, shape, myMotionState, body);
		physicsComponent.isDynamic = isDynamic;
	}

	void update(double dt) {
		dynamicsWorld->stepSimulation(dt / 1000.0);

		registry->view<Transform, Physics>().each([&](Transform& transform, Physics& physics) {
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

			btQuaternion rot = physics.body->getCenterOfMassTransform().getRotation();

			if (physics.applyRotation) {
				transform.rotationq = glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
			}
		});
	}
};