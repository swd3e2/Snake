#pragma once

#include "Model/Model.h"
#include "Model/ImportData.h"
#include "Common/Singleton.h"
#include "Model/gltf/GltfImporter.h"
#include "Common/Storage.h"
#include "TextureLoader.h"

class ModelLoader : public Singleton<ModelLoader> {
private:
	GltfImporter gltfImporter;
	Storage<Model> storage;
public:
	ModelLoader();
	std::shared_ptr<Model> loadFromFile(const std::string name);
private:
	void processNodes(const std::shared_ptr<Model>& model, const std::shared_ptr<Import::Model>& importModel);
	void processVertexArrays(const std::shared_ptr<Model>& model, const std::shared_ptr<Import::Model>& importModel);
	void processMaterials(std::shared_ptr<Model>& model, const std::shared_ptr<Import::Model>& importModel);
};