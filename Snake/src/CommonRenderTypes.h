#pragma once
#include <glm/glm.hpp>

struct WVP {
	glm::mat4 projection;
	glm::mat4 shadowProjection;
	glm::vec4 shadowCameraPos;
	glm::vec4 viewPos;
};

struct ModelData {
	glm::mat4 toWorld;
	glm::mat4 inverseToWorld;
	int selected = 0;
};

struct MaterialShaderData {
	glm::vec3 ambientColor = glm::vec3(1.0f, 0.0f, 1.0f);
	float shininess = 64.0f;
	glm::vec3 diffuseColor = glm::vec3(1.0f);
	float dummy;
	glm::vec3 specularColor = glm::vec3(1.0f);
	int hasDiffuseMap = 0;
	int hasNormalMap = 0;
};

struct MeshData {
	glm::mat4 transform;
	glm::mat4 normalTransform;
	MaterialShaderData materialData;
};