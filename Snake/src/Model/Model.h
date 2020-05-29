#pragma once

#include "ImportData.h"
#include "../Graphics/VertexBuffer.h"
#include "../Graphics/IndexBuffer.h"
#include "Material.h"

class Model {
public:
	/** Struct contains all information about current node transform */
	struct Transform {
		glm::vec3 translation;
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 rotationV;
		glm::mat4 worldTransform;
		glm::mat4 matrixTransform;
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
	Model() {}

	Model(const std::shared_ptr<Import::Model>& model) {
		rootNode = model->rootNode;
		filename = model->filename;
		skinned = model->skinned;

		for (int i = 0; i < model->nodes.size(); i++) {
			std::shared_ptr<Node> node = std::make_shared<Node>();
			node->id = model->nodes[i]->id;
			node->jointId = model->nodes[i]->jointId;
			node->mesh = model->nodes[i]->meshId;
			node->name = model->nodes[i]->name;

			node->childs = model->nodes[i]->childs;

			node->transform.rotation = model->nodes[i]->rotation;
			node->transform.rotationV = glm::eulerAngles(model->nodes[i]->rotation);

			node->transform.scale = model->nodes[i]->scale;
			node->transform.translation = model->nodes[i]->translation;

			nodes.push_back(node);
		}

		for (int i = 0; i < model->meshes.size(); i++) {
			std::shared_ptr<SubMesh> renderable = std::make_shared<SubMesh>();
			renderable->id = i;
			renderable->material = model->meshes[i]->material;

			renderable->vBuffer = std::make_shared<VertexBuffer>(model->meshes[i]->vertices.size(), sizeof(vertex), model->meshes[i]->vertices.data());
			renderable->iBuffer = std::make_shared<IndexBuffer>(model->meshes[i]->indices.size(), model->meshes[i]->indices.data());

			submeshes.push_back(renderable);
		}

	}

	const std::vector<std::shared_ptr<SubMesh>>& getSubMeshes() const {
		return submeshes;
	}

	const std::vector<std::shared_ptr<Node>>& getNodes() const {
		return nodes;
	}

	const int getRootNode() const { return rootNode; }
};