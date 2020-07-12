#pragma once

#include "Graphics/Renderer/VertexBuffer.h"
#include "Graphics/Renderer/IndexBuffer.h"
#include "Material.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <string>
#include <memory>
#include "Material.h"
#include <unordered_map>

/** Struct contains all information about node transform */
struct Transform {
	glm::vec3 translation;
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 rotationV;
	glm::mat4 worldTransform;
	glm::mat4 normalTransform;
	bool hasChanged = true;
};

/** Single drawable object */
struct SubMesh {
	int id;
	int material;
	std::shared_ptr<IndexBuffer> iBuffer;
	std::shared_ptr<VertexBuffer> vBuffer;
};

struct Node {
	int id;
	int jointId = -1;
	int mesh = -1;

	std::string name;
	Transform transform;
	std::vector<int> childs;
};

struct Model {
	int rootNode;

	std::string filename;
	bool skinned = false;
	bool selected = false;

	std::vector<std::shared_ptr<Node>> nodes;
	std::unordered_map<int, std::vector<std::shared_ptr<SubMesh>>> submeshes;
	std::vector<std::shared_ptr<Material>> materials;
};