#pragma once
#include <glm/glm.hpp>

struct WVP {
	glm::mat4 projection;
	glm::mat4 shadowProjection;
	glm::vec4 shadowCameraPos;
	glm::vec4 viewPos;
};