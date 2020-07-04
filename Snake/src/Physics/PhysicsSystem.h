#pragma once

#include <btBulletDynamicsCommon.h>
#include "Graphics/Physics/PhysicsDebugDraw.h"
#include <BulletCollision\CollisionDispatch\btCollisionWorld.h>

struct MyContactResultCallback : public btCollisionWorld::ContactResultCallback
{
	btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0Wrap,
		int partId0,
		int index0,
		const btCollisionObjectWrapper* colObj1Wrap,
		int partId1,
		int index1)
	{
		std::cout << 123 << std::endl;
		return 1;
	}
};

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

	Physics& createPhysicsBody(const entt::entity& entity, std::shared_ptr<btCollisionShape> shape, float mass = 0.0f, glm::vec3 originalPos = glm::vec3(0.0), bool isDynamic = false) {
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
		return physicsComponent;
	}

	void update(double dt) {
		dynamicsWorld->stepSimulation(dt / 1000.0);

		int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
		for (int i = 0; i < numManifolds; i++) {
			btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
			const btCollisionObject* obA = contactManifold->getBody0();
			const btCollisionObject* obB = contactManifold->getBody1();

			int numContacts = contactManifold->getNumContacts();
			for (int j = 0; j < numContacts; j++) {
				btManifoldPoint& pt = contactManifold->getContactPoint(j);
				if (pt.getDistance() < 0.f) {
					const btVector3& ptA = pt.getPositionWorldOnA();
					const btVector3& ptB = pt.getPositionWorldOnB();
					const btVector3& normalOnB = pt.m_normalWorldOnB;
					//printf("collision\n");
					bool x = (ContactProcessedCallback)(pt, obA, obB);
					if (x)
						printf("collision\n");
				}
			}
		}

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
				transform.rotation = glm::eulerAngles(transform.rotationq) * 3.14159f / 180.f;
			}

			transform.matrix = glm::translate(glm::mat4(1.0f), transform.translation);
			transform.matrix = transform.matrix * glm::toMat4(transform.rotationq);
		});
	}
};