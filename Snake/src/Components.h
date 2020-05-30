#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <string>
#include "Model/Model.h"

struct Transform {
	glm::vec3 translation;
	glm::vec3 rotation;
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
};

struct Render {
	std::shared_ptr<Model> model;
};

struct Script {
	std::string filename;
};

struct Player {

};

struct Animation {
	double animationTime = 0.0;
	int startX = 0;
	int startY = 8;

	int endX = 8;
	int endY = 7;
};