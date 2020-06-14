#pragma once

#include "RenderTarget.h"
#include "Graphics/Renderer/RenderTarget.h"

class MainRenderTarget : public RenderTarget {
public:
	virtual void setColorTexture(const std::shared_ptr<Texture2D>& texture, int slot, int level = 0) override {}
	virtual void removeColorTexture(int slot, int level) override {}
	virtual void setDepthTexture(const std::shared_ptr<Texture2D>& texture, int level = 0) override {}
};