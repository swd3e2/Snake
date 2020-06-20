#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <map>
#include <memory>
#include "../Graphics/vertex.h"


namespace Import {
	struct Node {
		int id;
		int jointId;
		int meshId = -1;
		std::string name;

		glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

		std::vector<int> childs;
	};

	struct AnimationData {
		std::map<double, glm::quat> rotations;
		std::map<double, glm::vec3> positions;
		std::map<double, glm::vec3> scale;
	};

	struct Animation {
		std::map<int, std::shared_ptr<AnimationData>> data;
		double duration = 0.0;
		std::string name;
	};

	struct Material {
		int id;

		std::string name;
		glm::vec4 baseColorFactor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		float metallicFactor = 0.0f;
		float roughnessFactor = 0.0f;
		float emissiveFactor = 0.0f;

		std::string diffuseTexture;
		std::string normalTexture;
		std::string specularTexture;
		std::string roughnesTexture;
		std::string emisiveTexture;
		std::string occlusionTexture;
	};

	struct Mesh {
		int id;
		int material;
		bool hasTangent = false;
		std::string name;
		std::vector<vertex> vertices;
		std::vector<unsigned int> indices;
	};

	struct Model {
		std::string filename;
		std::vector <std::shared_ptr<Mesh >> meshes;
		std::vector<Material> materials;
		// Animation data
		std::vector<int> joints;
		std::vector<std::shared_ptr<Animation>> animations;
		std::vector<glm::mat4> inverseBindMatrices;
		int rootJoint;
		bool skinned = false;
		// Node hierarchy data
		int rootNode;
		std::vector<std::shared_ptr<Node>> nodes;
		// A - left near down, B - right far up
		glm::vec3 A;
		glm::vec3 B;
	};
}
