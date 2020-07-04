#pragma once

#include "Graphics/Renderer.h"

class CustomInterface {
private:
	Renderer* renderer;
public:
	CustomInterface(Renderer* renderer) 
		: renderer(renderer)
	{

	}
};