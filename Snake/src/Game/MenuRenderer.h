#pragma once

#include "../Graphics/2DRenderer.h"
#include <memory>

class MenuRenderer {
private:
	std::shared_ptr<Renderer2D> renderer;
public:
	MenuRenderer(const std::shared_ptr<Renderer2D>& renderer) :
		renderer(renderer)
	{
	}

	void render() {

	}
};