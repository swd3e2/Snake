#pragma once

#include <glm/glm.hpp>

struct vertex
{
	vertex(float px = 0.0f, float py = 0.0f, float pz = 0.0f,
		float tv = 0.0f, float tu = 0.0f,
		float nx = 0.0f, float ny = 0.0f, float nz = 0.0f)
		: pos(px, py, pz),
		texCoord(tv, tu),
		normal(nx, ny, nz),
		tangent(0.0f, 0.0f, 0.0f),
		bitangent(0.0f, 0.0f, 0.0f)
	{}

	glm::vec3 pos;
	glm::vec2 texCoord;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;

	struct VertexBoneData
	{
		int joints[4]{};
		float weights[4]{};
	} boneData;
};