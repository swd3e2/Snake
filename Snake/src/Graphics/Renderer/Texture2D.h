#pragma once

#include <string>
#include <stb_image.h>
#include "Graphics/Bindable.h"
#include "CommonTypes.h"

class Texture2D : public Bindable {
public:
    int width = 0;
    int height = 0;
	int depth = 0;
	int location = 0; // Texture slot
	int numMips = 1;
	int samples = 0;
	TextureFormat textureFormat;
public:
	Texture2D(int width, int height, int location, TextureFormat textureFormat, int numMips) :
		width(width), height(height), location(location), textureFormat(textureFormat)
	{}

	static Texture2D* create(int width, int height, int location, void* data, TextureFormat textureFormat = TextureFormat::RGBA8, int numMips = 0);

	const TextureFormat getFormat() const { return textureFormat; }
	const int getWidth() const { return width; }
	const int getHeight() const { return height; }
	const int getLocation() const { return location; }
	void setLocation(const int location) { this->location = location; }

	virtual void setData(void* data) = 0;
	virtual void generateMips() = 0;
	virtual void bindToUnit(const int location, RenderContext* renderContext) = 0;
};