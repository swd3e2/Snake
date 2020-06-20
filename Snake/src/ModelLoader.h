#pragma once

#include "Model/Model.h"
#include <memory>
#include <string>
#include "Singleton.h"
#include "Model/Import/GltfImporter.h"
#include "Import/RawTexture.h"
#include "Storage.h"
#include "TextureLoader.h"

class ModelLoader : public Singleton<ModelLoader> {
private:
	GltfImporter gltfImporter;
	Storage<Model> storage;
public:
	ModelLoader() {
		storage.startUp();
	}

	std::shared_ptr<Model> loadFromFile(const std::string name) {
		std::shared_ptr<Import::Model> importModel = gltfImporter.import(name);
		std::shared_ptr<Model> model = std::make_shared<Model>();

		model->rootNode = importModel->rootNode;
		model->filename = importModel->filename;
		model->skinned = importModel->skinned;

		processNodes(model, importModel);
		processVertexArrays(model, importModel);
		processMaterials(model, importModel);

		return model;
	}
private:
	void processNodes(const std::shared_ptr<Model>& model, const std::shared_ptr<Import::Model>& importModel) {
		for (int i = 0; i < importModel->nodes.size(); i++) {
			std::shared_ptr<Model::Node> node = std::make_shared<Model::Node>();
			node->id = importModel->nodes[i]->id;
			node->jointId = importModel->nodes[i]->jointId;
			node->mesh = importModel->nodes[i]->meshId;
			node->name = importModel->nodes[i]->name;

			node->childs = importModel->nodes[i]->childs;

			node->transform.rotation = importModel->nodes[i]->rotation;
			node->transform.rotationV = glm::eulerAngles(importModel->nodes[i]->rotation);

			node->transform.scale = importModel->nodes[i]->scale;
			node->transform.translation = importModel->nodes[i]->translation;

			model->nodes.push_back(node);
		}
	}

	void processVertexArrays(const std::shared_ptr<Model>& model, const std::shared_ptr<Import::Model>& importModel) {
		for (int i = 0; i < importModel->meshes.size(); i++) {
			std::shared_ptr<Model::SubMesh> renderable = std::make_shared<Model::SubMesh>();
			renderable->id = i;
			renderable->material = importModel->meshes[i]->material;

			renderable->vBuffer.reset(VertexBuffer::create(importModel->meshes[i]->vertices.size(), sizeof(vertex), importModel->meshes[i]->vertices.data()));
			renderable->iBuffer.reset(IndexBuffer::create(importModel->meshes[i]->indices.size(), importModel->meshes[i]->indices.data()));

			model->submeshes.push_back(renderable);
		}
	}

	void processMaterials(std::shared_ptr<Model>& model, const std::shared_ptr<Import::Model>& importModel) {
		for (auto& it : importModel->materials) {
			std::shared_ptr<Model::Material> material = std::make_shared<Model::Material>();
			material->name = it.name;
			material->materialData.color = it.baseColorFactor;

			material->materialData.metallicFactor = it.metallicFactor;
			material->materialData.roughnessFactor = it.roughnessFactor;
			material->materialData.emissiveFactor = it.emissiveFactor;

			material->diffuseTexture = TextureLoader::instance()->loadFromFile(it.diffuseTexture);
			material->normalTexture = TextureLoader::instance()->loadFromFile(it.normalTexture);

			model->materials.push_back(material);
		}
	}
};