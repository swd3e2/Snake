#pragma once

#include <memory>
#include "../ImportData.h"

#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <nlohmann/json.hpp>
#include <tiny_gltf.h>

#include <glm/gtx/matrix_decompose.hpp>
#include <fstream>
#include "../../Common/Helper.h"

class GltfImporter
{
private:
	int animationNodeCounter = 0;
	int meshCounter = 0;
	int nodeCounter = 0;
public:
	std::shared_ptr<Import::Model> import(const char* filename);
private:
	void calculateTangent(Import::Model* model);
	void processHierarchy(Import::Model* model, const tinygltf::Model& gltfModel);
	void processAnimations(Import::Model* lModel, tinygltf::Model& model, std::ifstream& file);
	void processSkin(Import::Model* model, const tinygltf::Model& gltfModel, std::ifstream& binary);
	void processMeshes(Import::Model* model, const tinygltf::Model& gltfModel, std::ifstream& binary);
	void processMaterials(Import::Model* model, const tinygltf::Model& gltfModel);
	int getSkinByMesh(int mesh, const tinygltf::Node& node, const tinygltf::Model& model);
	std::string getBinaryFileFolder(std::string gltfFilePath);
	int getJointByNode(int nodeId, const tinygltf::Model& gltfModel);
	int getNodeByJoint(int jointId, const tinygltf::Model& gltfModel);
	bool hasMesh(const tinygltf::Node& node, const tinygltf::Model& model);
	void getTransformMatrix(const tinygltf::Node& modelNode, const std::shared_ptr<Import::Node>& node);
};