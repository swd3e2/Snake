#pragma once

#include "Graphics/Renderer/Texture2D.h"
#include "DirectXRenderer.h"

class DirectXTexture2D : public Texture2D {
public:
	DirectXTexture2D(int width, int height, int location, void* data, TextureFormat textureFormat, int numMips) :
		Texture2D(width, height, location, textureFormat, numMips)
	{

	}

	virtual void bind(RenderContext* renderContext) override {

	}

	virtual void setData(void* data) override {

	}

	virtual void generateMips() override {

	}

	virtual void bindToUnit(const int location, RenderContext* renderContext) override {

	}
};