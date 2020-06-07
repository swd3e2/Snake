#pragma once

#include <string>
#include <stb_image.h>
#include <GL/glew.h>
#include "Graphics/Bindable.h"
#include "CommonTypes.h"

class Texture2D : public Bindable {
public:
    int width = 0;
    int height = 0;
	int location = 0; // Texture slot
	TextureFormat textureFormat;
public:
	Texture2D(int width, int height, int location, TextureFormat textureFormat) :
		width(width), height(height), location(location)
	{}

	static Texture2D* create(int width, int height, int location, void* data, TextureFormat textureFormat = TextureFormat::RGBA8);

	const TextureFormat getFormat() const { return textureFormat; }
	const int getWidth() const { return width; }
	const int getHeight() const { return height; }
	const int getLocation() const { return location; }
};