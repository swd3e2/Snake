#include "ModelLoader.h"
#include "Model/gltf/GltfImporter.h"

ModelLoader::ModelLoader() {
	auto ptr = std::make_shared<GltfImporter>();
	importers.push_back(ptr);
}

std::shared_ptr<Model> ModelLoader::loadFromFile(const std::string filename) {
	std::string fullFilePath = filename;

	if (!File(fullFilePath)) {
		const std::string assetFolder = getAssetFolder();
		fullFilePath = assetFolder + "/" + filename;
	}

	if (storage.has(fullFilePath)) {
		return storage.get(fullFilePath);
	}

	if (!canImport(fullFilePath)) {
		return nullptr;
	}

	const std::shared_ptr<ModelImporter>& impoter = getSuitableImporter(fullFilePath);
	std::shared_ptr<Import::Model> importModel = impoter->import(fullFilePath);
	std::shared_ptr<Model> model = std::make_shared<Model>();

	model->rootNode = importModel->rootNode;
	model->filename = importModel->filename;
	model->skinned = importModel->skinned;

	processNodes(model, importModel);
	processVertexArrays(model, importModel);
	processMaterials(model, importModel);

	storage.set(fullFilePath, model);

	return model;
}

void ModelLoader::processNodes(const std::shared_ptr<Model>& model, const std::shared_ptr<Import::Model>& importModel) {
	for (int i = 0; i < importModel->nodes.size(); i++) {
		std::shared_ptr<Node> node = std::make_shared<Node>();
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

void ModelLoader::processVertexArrays(const std::shared_ptr<Model>& model, const std::shared_ptr<Import::Model>& importModel) {
	int renderableCounter = -1;

	for (auto& meshId : importModel->meshes) {
		for (auto& mesh : meshId.second) {
			std::shared_ptr<SubMesh> renderable = std::make_shared<SubMesh>();
			renderable->id = ++renderableCounter;
			renderable->material = mesh->material;

			renderable->vBuffer.reset(VertexBuffer::create(mesh->vertices.size(), sizeof(vertex), mesh->vertices.data()));
			renderable->iBuffer.reset(IndexBuffer::create(mesh->indices.size(), mesh->indices.data()));

			model->submeshes[meshId.first].push_back(renderable);
		}
	}
}

void ModelLoader::processMaterials(std::shared_ptr<Model>& model, const std::shared_ptr<Import::Model>& importModel) {
	for (auto& it : importModel->materials) {
		std::shared_ptr<Material> material = std::make_shared<Material>();
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

const std::string ModelLoader::getAssetFolder() {
	if (Configuration::instance()->has("assets_dir") && assetFolder.size() == 0) {
		assetFolder = Configuration::instance()->get("assets_dir");
	}

	return assetFolder;
}

bool ModelLoader::canImport(const std::string& filename) {
	for (auto& it : importers) {
		if (it->canParseFile(filename)) {
			return true;
		}
	}
	return false;
}

const std::shared_ptr<ModelImporter>& ModelLoader::getSuitableImporter(const std::string& filename) {
	for (auto& it : importers) {
		if (it->canParseFile(filename)) {
			return it;
		}
	}
	return nullptr;
}