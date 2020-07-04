#pragma once

#include "RenderTarget.h"
#include "Graphics/Renderer/RenderTarget.h"

class MainRenderTarget : public RenderTarget {
private:
	int width;
	int height;
public:
	MainRenderTarget(int width, int height) :
		width(width), height(height)
	{}

	virtual void setColorTexture(const std::shared_ptr<Texture2D>& texture, int slot, int level = 0) override {}
	virtual void setDepthTexture(const std::shared_ptr<Texture2D>& texture, int level = 0) override {}


	virtual int getWidth() override {
		return width;
	}

	virtual int getHeight() {
		return height;
	}
};