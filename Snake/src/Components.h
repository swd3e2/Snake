#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <string>
#include "Model/Model.h"
#include <btBulletDynamicsCommon.h>

struct Transform {
	glm::vec3 translation;
	glm::vec3 rotation;
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::quat rotationq = glm::quat(0.0, 0.0, 0.0, 0.0);
	glm::mat4 matrix = glm::mat4(1.0f);
};

struct Render {
	std::shared_ptr<Model> model;
};

struct Script {
	std::string filename;
};

struct PlayerComponent {
	bool active = false;
	float speed = 0.1f;
};

struct Animation {
	double animationTime = 0.0;
	int startX = 0;
	int startY = 8;

	int endX = 8;
	int endY = 7;
};

struct Physics {
	std::shared_ptr<btCollisionShape> shape;
	std::shared_ptr<btDefaultMotionState> myMotionState;
	std::shared_ptr<btRigidBody> body;
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
};

struct CameraComponent {
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::vec4 forwardVec;
	glm::vec4 rightVec;
};

struct Light {
	glm::vec3 color;
};