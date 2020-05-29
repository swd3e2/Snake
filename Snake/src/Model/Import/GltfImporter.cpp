#include "GltfImporter.h"

std::shared_ptr<Import::Model> GltfImporter::import(const char* filename)
{
	meshCounter = nodeCounter = animationNodeCounter = 0;

	tinygltf::Model gltfModel;
	tinygltf::TinyGLTF loader;

	if (!loader.LoadASCIIFromFile(&gltfModel, nullptr, nullptr, filename)) {
		return nullptr;
	}

	Import::Model* model = new Import::Model();
	model->filename = filename;

	std::string binaryFilePath = getBinaryFileFolder(filename) + gltfModel.buffers[0].uri;
	std::ifstream binary(binaryFilePath, std::fstream::in | std::fstream::binary);

	if (!binary.is_open()) {
		delete model;
		return nullptr;
	}

	tinygltf::Node& modelRootNode = gltfModel.nodes[gltfModel.scenes[0].nodes[0]];
	model->rootNode = 0;// gltfModel.scenes[0].nodes[0];

	processHierarchy(model, gltfModel);
	processSkin(model, gltfModel, binary);
	processMaterials(model, gltfModel);
	processMeshes(model, gltfModel, binary);
	processAnimations(model, gltfModel, binary);

	binary.close();

	calculateTangent(model);

	return std::shared_ptr<Import::Model>(model);
}

void GltfImporter::processMeshes(Import::Model* model, const tinygltf::Model& gltfModel, std::ifstream& binary)
{
	for (int i = 0; i < gltfModel.meshes.size(); i++) {
		const tinygltf::Mesh& gltfMesh = gltfModel.meshes[i];

		for (auto& primitive : gltfMesh.primitives) {
			std::shared_ptr<Import::Mesh> mesh = std::make_shared<Import::Mesh>();
			mesh->material = primitive.material;
			mesh->name = gltfMesh.name.size() ? gltfMesh.name : ("Mesh " + std::to_string(meshCounter));
			mesh->id = meshCounter++;

			for (auto& attribute : primitive.attributes) {
				const tinygltf::Accessor& accessor = gltfModel.accessors[attribute.second];
				const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
				binary.seekg(bufferView.byteOffset + accessor.byteOffset, std::fstream::beg);

				if (mesh->vertices.size() < accessor.count) {
					mesh->vertices.resize(accessor.count);
				}

				if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
					if (attribute.first == "NORMAL") {
						float* data = new float[accessor.count * 3];
						binary.read((char*)data, sizeof(float) * accessor.count * 3);

						for (int i = 0; i < accessor.count; i++) {
							mesh->vertices[i].normal.x = data[i * 3];
							mesh->vertices[i].normal.y = data[i * 3 + 1];
							mesh->vertices[i].normal.z = data[i * 3 + 2];
						}
						delete[] data;
					} else if (attribute.first == "POSITION") {
						float* data = new float[accessor.count * 3];
						binary.read((char*)data, sizeof(float) * accessor.count * 3);

						for (int i = 0; i < accessor.count; i++) {
							mesh->vertices[i].pos.x = data[i * 3];
							mesh->vertices[i].pos.y = data[i * 3 + 1];
							mesh->vertices[i].pos.z = data[i * 3 + 2];
						}
						delete[] data;
					} else if (attribute.first == "WEIGHTS_0") {
						float* data = new float[accessor.count * 4];
						binary.read((char*)data, sizeof(float) * accessor.count * 4);

						for (int i = 0; i < accessor.count; i++) {
							mesh->vertices[i].boneData.weights[0] = data[i * 4];
							mesh->vertices[i].boneData.weights[1] = data[i * 4 + 1];
							mesh->vertices[i].boneData.weights[2] = data[i * 4 + 2];
							mesh->vertices[i].boneData.weights[3] = data[i * 4 + 3];
						}
						delete[] data;
					} else if (attribute.first == "TEXCOORD_0") {
						float* data = new float[accessor.count * 2];
						binary.read((char*)data, sizeof(float) * accessor.count * 2);

						for (int i = 0; i < accessor.count; i++) {
							mesh->vertices[i].texCoord.x = data[i * 2];
							mesh->vertices[i].texCoord.y = data[i * 2 + 1];
						}
						delete[] data;
					} else if (attribute.first == "TANGENT") {
						mesh->hasTangent = true;

						float* data = new float[accessor.count * 4];
						binary.read((char*)data, sizeof(float) * accessor.count * 4);

						for (int i = 0; i < accessor.count; i++) {
							mesh->vertices[i].tangent = glm::eulerAngles(glm::quat(data[i * 4 + 3], data[i * 4], data[i * 4 + 1], data[i * 4 + 2]));
						}
						delete[] data;
					}
				} else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
					if (attribute.first == "JOINTS_0") {
						unsigned short* data = new unsigned short[accessor.count * 4];
						binary.read((char*)data, sizeof(unsigned short) * accessor.count * 4);

						for (int i = 0; i < accessor.count; i++) {
							mesh->vertices[i].boneData.joints[0] = data[i * 4 + 0];
							mesh->vertices[i].boneData.joints[1] = data[i * 4 + 1];
							mesh->vertices[i].boneData.joints[2] = data[i * 4 + 2];
							mesh->vertices[i].boneData.joints[3] = data[i * 4 + 3];
						}

						delete[] data;
					}
				}
			}

			// Indices
			const tinygltf::Accessor& accessor = gltfModel.accessors[primitive.indices];
			const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
			binary.seekg(bufferView.byteOffset + accessor.byteOffset, std::fstream::beg);

			if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
				unsigned short* data = new unsigned short[accessor.count];
				binary.read((char*)data, sizeof(unsigned short) * accessor.count);

				for (int i = 0; i < accessor.count; i++) {
					mesh->indices.push_back(data[i]);
				}
				delete[] data;
			} else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
				unsigned int* data = new unsigned int[accessor.count];
				binary.read((char*)data, sizeof(unsigned int) * accessor.count);

				for (int i = 0; i < accessor.count; i++) {
					mesh->indices.push_back(data[i]);
				}
				delete[] data;
			}

			model->meshes.push_back(mesh);
		}
	}
}

void GltfImporter::processMaterials(Import::Model* model, const tinygltf::Model& gltfModel)
{
	for (int i = 0; i < gltfModel.materials.size(); i++) {
		const tinygltf::Material& gltfMaterial = gltfModel.materials[i];

		Import::Material material;

		material.id = i;
		material.name = gltfMaterial.name;

		for (auto& value : gltfMaterial.values) {
			if (value.first == "baseColorTexture") {
				material.diffuseTexture = gltfModel.images[gltfModel.textures[value.second.TextureIndex()].source].uri;
			} else if (value.first == "metallicRoughnessTexture") {
				material.roughnesTexture = gltfModel.images[gltfModel.textures[value.second.TextureIndex()].source].uri;
			} else if (value.first == "metallicFactor") {
				material.metallicFactor = value.second.number_value;
			} else if (value.first == "roughnessFactor") {
				material.roughnessFactor = value.second.number_value;
			} else if (value.first == "baseColorFactor") {
				material.baseColorFactor = glm::vec4(
					value.second.number_array[0],
					value.second.number_array[1],
					value.second.number_array[2],
					value.second.number_array[3]
				);
			}
		}

		for (auto& value : gltfMaterial.additionalValues) {
			if (value.first == "emisiveTexture") {
				material.emisiveTexture = gltfModel.images[gltfModel.textures[value.second.TextureIndex()].source].uri;
			} else if (value.first == "normalTexture") {
				material.normalTexture = gltfModel.images[gltfModel.textures[value.second.TextureIndex()].source].uri;
			} else if (value.first == "occlusionTexture") {
				material.occlusionTexture = gltfModel.images[gltfModel.textures[value.second.TextureIndex()].source].uri;
			} else if (value.first == "emissiveFactor") {
				material.emissiveFactor = value.second.number_value;
			}
		}

		for (auto& it : gltfMaterial.extensions) {
			std::vector<std::string> keys = it.second.Keys();
			for (std::string& key : keys) {
				auto& value = it.second.Get(key);
				if (value.IsObject()) {
					std::vector<std::string> innerKeys = value.Keys();
					for (std::string& innerKey : innerKeys) {
						if (key == "diffuseTexture" && innerKey == "index") {
							tinygltf::Value index = value.Get(innerKey);
							int val = index.Get<int>();
							material.diffuseTexture = gltfModel.images[gltfModel.textures[val].source].uri;
						} else if (key == "specularGlossinessTexture" && innerKey == "index") {
							tinygltf::Value index = value.Get(innerKey);
							int val = index.Get<int>();
							material.specularTexture = gltfModel.images[gltfModel.textures[val].source].uri;
						}
					}
				}
			}
		}
		model->materials.push_back(material);
	}
}

void GltfImporter::processSkin(Import::Model* model, const tinygltf::Model& gltfModel, std::ifstream& binary)
{
	if (gltfModel.skins.size() > 0) {
		model->skinned = true;

		const tinygltf::Accessor& accessor = gltfModel.accessors[gltfModel.skins[0].inverseBindMatrices];
		const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
		binary.seekg(bufferView.byteOffset + accessor.byteOffset, std::fstream::beg);

		glm::mat4* matrixData = new glm::mat4[gltfModel.skins[0].joints.size()];
		binary.read((char*)matrixData, sizeof(glm::mat4) * accessor.count);

		model->rootJoint = gltfModel.skins[0].skeleton;
		model->joints = gltfModel.skins[0].joints;

		for (int i = 0; i < gltfModel.skins[0].joints.size(); i++) {
			model->inverseBindMatrices.push_back(glm::transpose(matrixData[i]));
		}

		delete[] matrixData;
	}
}

/**
	* Loads keyframes for animation
	*/
void GltfImporter::processAnimations(Import::Model* model, tinygltf::Model& gltfModel, std::ifstream& binary)
{
	for (auto& animation : gltfModel.animations) {
		std::shared_ptr<Import::Animation> mAnimation = std::make_shared<Import::Animation>();
		mAnimation->name = animation.name;

		for (tinygltf::AnimationChannel& channel : animation.channels) {
			tinygltf::AnimationSampler& sampler = animation.samplers[channel.sampler];
			tinygltf::Accessor& timeAccessor = gltfModel.accessors[sampler.input];
			tinygltf::BufferView& timeBufferView = gltfModel.bufferViews[timeAccessor.bufferView];
			binary.seekg(timeBufferView.byteOffset + timeAccessor.byteOffset, std::fstream::beg);

			float* timeData = new float[timeAccessor.count];
			binary.read((char*)timeData, sizeof(float) * timeAccessor.count);

			if (timeData[timeAccessor.count - 1] > mAnimation->duration) {
				mAnimation->duration = timeData[timeAccessor.count - 1];
			}

			tinygltf::Accessor& valueAccessor = gltfModel.accessors[sampler.output];
			tinygltf::BufferView& valueBufferView = gltfModel.bufferViews[valueAccessor.bufferView];
			binary.seekg(valueBufferView.byteOffset + valueAccessor.byteOffset, std::fstream::beg);

			float* valueData = nullptr;
			if (channel.target_path == "rotation") {
				valueData = new float[valueAccessor.count * 4];
				binary.read((char*)valueData, sizeof(float) * valueAccessor.count * 4);
			} else {
				valueData = new float[valueAccessor.count * 3];
				binary.read((char*)valueData, sizeof(float) * valueAccessor.count * 3);
			}

			std::shared_ptr<Import::AnimationData> data;

			if (mAnimation->data.find(channel.target_node) != mAnimation->data.end()) {
				data = mAnimation->data[channel.target_node];
			} else {
				data = std::make_shared<Import::AnimationData>();
				mAnimation->data[channel.target_node] = data;
			}

			if (channel.target_path == "rotation") {
				for (int i = 0; i < timeAccessor.count; i++) {
					data->rotations[timeData[i]] = glm::quat(valueData[i * 4 + 3], valueData[i * 4], valueData[i * 4 + 1], valueData[i * 4 + 2]);
				}
			} else if (channel.target_path == "translation") {
				for (int i = 0; i < timeAccessor.count; i++) {
					data->positions[timeData[i]] = glm::vec3(valueData[i * 3], valueData[i * 3 + 1], valueData[i * 3 + 2]);
				}
			} else if (channel.target_path == "scale") {
				for (int i = 0; i < timeAccessor.count; i++) {
					data->scale[timeData[i]] = glm::vec3(valueData[i * 3], valueData[i * 3 + 1], valueData[i * 3 + 2]);
				}
			}

			delete[] timeData;
			delete[] valueData;
		}
		model->animations.push_back(mAnimation);
	}
}

void GltfImporter::calculateTangent(Import::Model* model)
{
	for (auto& it : model->meshes) {
		if (it->hasTangent) {
			for (int i = 0; i < it->indices.size(); i++) {
				it->vertices[it->indices[i]].bitangent = glm::cross(it->vertices[it->indices[i]].normal, it->vertices[it->indices[i]].tangent);
			}
		} else {
			for (int i = 0; i < it->indices.size() - 2; i += 3) {
				vertex& first = it->vertices[it->indices[i + 0]];
				vertex& second = it->vertices[it->indices[i + 1]];
				vertex& third = it->vertices[it->indices[i + 2]];

				glm::vec3 edge1 = second.pos - first.pos;
				glm::vec3 edge2 = third.pos - first.pos;

				glm::vec2 uv1 = second.texCoord - first.texCoord;
				glm::vec2 uv2 = third.texCoord - first.texCoord;

				float f = 1.0f / uv1.x * uv2.y - uv2.x * uv1.y;

				glm::vec3 tangent(
					f * (uv2.y * edge1.x - uv1.y * edge2.x),
					f * (uv2.y * edge1.y - uv1.y * edge2.y),
					f * (uv2.y * edge1.z - uv1.y * edge2.z)
				);

				glm::vec3 bitangent(
					f * (uv1.x * edge2.x - uv2.x * edge1.x),
					f * (uv1.x * edge2.y - uv2.x * edge1.y),
					f * (uv1.x * edge2.z - uv2.x * edge1.z)
				);

				first.tangent += tangent;
				second.tangent += tangent;
				third.tangent += tangent;

				first.bitangent += bitangent;
				second.bitangent += bitangent;
				third.bitangent += bitangent;
			}
		}

		for (auto& vertex : it->vertices) {
			vertex.bitangent = glm::normalize(vertex.bitangent);
			vertex.tangent = glm::normalize(vertex.tangent);
		}
	}
}

/**
* Creates hierarchy of nodes
*/
void GltfImporter::processHierarchy(Import::Model* model, const tinygltf::Model& gltfModel)
{
	for (int i = 0; i < gltfModel.nodes.size(); i++) {
		const tinygltf::Node& gltfNode = gltfModel.nodes[i];
		std::shared_ptr<Import::Node> node = std::make_shared<Import::Node>();

		node->id = i;
		node->jointId = getJointByNode(node->id, gltfModel);
		node->meshId = gltfNode.mesh;
		node->childs = gltfNode.children;

		node->name = gltfNode.name.size() > 0 ? gltfNode.name : ("Node " + std::to_string(nodeCounter++));

		model->nodes.push_back(node);
		getTransformMatrix(gltfNode, node);
	}
}

int GltfImporter::getSkinByMesh(int mesh, const tinygltf::Node& node, const tinygltf::Model& model)
{
	if (node.mesh == mesh) {
		return node.skin;
	}

	if (node.children.size() == 0) {
		return -1;
	} else {
		int result = -1;

		for (auto it : node.children) {
			result = getSkinByMesh(mesh, model.nodes[it], model);
			if (result >= 0) {
				return result;
			}
		}
		return -1;
	}
}

std::string GltfImporter::getBinaryFileFolder(std::string gltfFilePath)
{
	std::replace(gltfFilePath.begin(), gltfFilePath.end(), '\\', '/');
	gltfFilePath = Helper::trimToLastLineEntry(gltfFilePath.c_str(), '/');
	return gltfFilePath;
}

int GltfImporter::getJointByNode(int nodeId, const tinygltf::Model& gltfModel)
{
	if (gltfModel.skins.size() == 0) {
		return -1;
	}

	const std::vector<int>& joints = gltfModel.skins[0].joints;

	int lastJointId = 0;
	for (int i = 0; i < joints.size(); i++) {
		if (joints[i] == nodeId) return i;
	}

	return -1;
}

int GltfImporter::getNodeByJoint(int jointId, const tinygltf::Model& gltfModel)
{
	const std::vector<int>& joints = gltfModel.skins[0].joints;

	if (jointId < joints.size()) {
		return joints[jointId];
	}

	return -1;
}

void GltfImporter::getTransformMatrix(const tinygltf::Node& modelNode, const std::shared_ptr<Import::Node>& node)
{
	if (modelNode.matrix.data()) {
		glm::mat4 transform = glm::mat4(
			modelNode.matrix[0], modelNode.matrix[1], modelNode.matrix[2], modelNode.matrix[3],
			modelNode.matrix[4], modelNode.matrix[5], modelNode.matrix[6], modelNode.matrix[7],
			modelNode.matrix[8], modelNode.matrix[9], modelNode.matrix[10], modelNode.matrix[11],
			modelNode.matrix[12], modelNode.matrix[13], modelNode.matrix[14], modelNode.matrix[15]
		);

		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;

		glm::decompose(transform, node->scale, node->rotation, node->translation, skew, perspective);
	} else {
		if (modelNode.translation.size() > 0) {
			node->translation = glm::vec3(modelNode.translation[0], modelNode.translation[1], modelNode.translation[2]);
		}
		if (modelNode.rotation.size() > 0) {
			node->rotation = glm::quat(modelNode.rotation[3], modelNode.rotation[0], modelNode.rotation[1], modelNode.rotation[2]);
		}
		if (modelNode.scale.size() > 0) {
			node->scale = glm::vec3(modelNode.scale[0], modelNode.scale[1], modelNode.scale[2]);
		}
	}
}

bool GltfImporter::hasMesh(const tinygltf::Node& node, const tinygltf::Model& model)
{
	if (node.mesh >= 0) {
		return true;
	}

	if (node.children.size() == 0) {
		return false;
	}

	for (auto& child : node.children) {
		if (hasMesh(model.nodes[child], model)) {
			return true;
		}
	}
	return false;
}
