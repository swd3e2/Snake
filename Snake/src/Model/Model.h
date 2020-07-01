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

class Model {
public:
	friend class ModelLoader;

	/** Struct contains all information about current node transform */
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
private:
	int rootNode;

	std::vector<std::shared_ptr<Node>> nodes;
	std::vector<std::shared_ptr<SubMesh>> submeshes;
	std::vector<std::shared_ptr<Material>> materials;
	
	std::string filename;
	bool skinned = false;
public:
	const std::vector<std::shared_ptr<SubMesh>>& getSubMeshes() const { return submeshes; }
	const std::vector<std::shared_ptr<Node>>& getNodes() const { return nodes; }
	const std::vector<std::shared_ptr<Material>>& getMaterials() const { return materials; }
	const int getRootNode() const { return rootNode; }
	const std::string getImportFilename() const { return filename; }
};