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
#include "Graphics/Renderer/Texture2D.h"

class Model {
public:
	friend class ModelLoader;
	struct MaterialData {
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0, 1.0f);
		float metallicFactor;
		float roughnessFactor;
		float emissiveFactor;
		float ao;
		int hasDiffuseTexture = 0;
		int hasNormalTexture = 0;
		int hasSpecularMap = 0;
		int pad;
	};

	struct Material {
		std::string name;
		MaterialData materialData;

		std::shared_ptr<Texture2D> diffuseTexture;
		std::shared_ptr<Texture2D> normalTexture;
		std::shared_ptr<Texture2D> specularTexture;
		std::shared_ptr<Texture2D> roughnesTexture;
		std::shared_ptr<Texture2D> emisiveTexture;
		std::shared_ptr<Texture2D> metallicTexture;
		std::shared_ptr<Texture2D> occlusionTexture;
	};

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
	const int getRootNode() const { return rootNode; }
	const std::string getImportFilename() const { return filename; }
};