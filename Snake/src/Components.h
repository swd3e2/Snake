#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <string>
#include "Model/Model.h"
#include <btBulletDynamicsCommon.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <BulletCollision\CollisionDispatch\btCollisionWorld.h>
#include <BulletCollision\CollisionDispatch\btGhostObject.h>

struct TransformComponent {
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::quat rotationq = glm::quat(0.0, 0.0, 0.0, 0.0);
	glm::mat4 matrix = glm::mat4(1.0f);
};

struct RenderComponent {
	std::shared_ptr<Model> model;
	bool selected = false;
};

struct ScriptComponent {
	std::string filename;
};

struct PlayerComponent {
	bool active = false;
	float speed = 5.0f;
	glm::vec3 m_manualVelocity;
};

struct PlayerPhysics {
	std::shared_ptr<btCollisionShape> shape;
	std::shared_ptr<btDefaultMotionState> myMotionState;
	std::shared_ptr<btRigidBody> body;
	glm::vec3 originPosition;
};

struct PhysicsComponent {
	btCollisionShape* shape;
	btDefaultMotionState* myMotionState;
	btRigidBody* body;
	btPairCachingGhostObject* m_pGhostObject;

	//PhysicsShapeType type;
	glm::vec3 originPosition;
	glm::vec3 boxSize;
	glm::vec3 planeNormal;
	glm::vec3 localScale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 angularFactor = glm::vec3(1.0f, 1.0f, 1.0f);
	float contactingThreshold;
	float capsuleHeight;
	float capsuleRadius;
	float planeSize;
	float sphereRadius;
	bool applyRotation = true;
	bool isDynamic = false;
	float mass = 0.0f;
	bool m_onGround = true;
};

struct CameraComponent {
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::vec4 forwardVec;
	glm::vec4 rightVec;
};

struct LightComponent {
	glm::vec3 color;
};

struct MovingBoxComponent {
	bool moveUp = false;
};