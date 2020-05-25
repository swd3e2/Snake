#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <string>

struct Position {
	float x = 0.0f;
	float y = 0.0f;
};

struct Render {
	int xIndex = 0;
	int yIndex = 0;
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