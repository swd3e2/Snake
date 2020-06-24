#pragma once

#include "Model/Model.h"
#include "Model/ImportData.h"
#include "Common/Singleton.h"
#include "Model/ModelImporter.h"
#include "Common/Storage.h"
#include "TextureLoader.h"

class ModelLoader : public Singleton<ModelLoader> {
private:
	std::vector<std::shared_ptr<ModelImporter>> importers;
	Storage<Model> storage;
	std::string assetFolder;
public:
	std::shared_ptr<Model> loadFromFile(const std::string filename);
	bool canImport(const std::string& filename);
public:
	ModelLoader();
private:
	void processNodes(const std::shared_ptr<Model>& model, const std::shared_ptr<Import::Model>& importModel);
	void processVertexArrays(const std::shared_ptr<Model>& model, const std::shared_ptr<Import::Model>& importModel);
	void processMaterials(std::shared_ptr<Model>& model, const std::shared_ptr<Import::Model>& importModel);
	const std::string getAssetFolder();

	/**
	 * Returns importer for given file
	 */
	const std::shared_ptr<ModelImporter>& getSuitableImporter(const std::string& filename);
};