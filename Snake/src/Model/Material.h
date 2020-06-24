#pragma once

#include <glm/glm.hpp>
#include <string>
#include <memory>
#include "Graphics/Renderer/Texture2D.h"

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