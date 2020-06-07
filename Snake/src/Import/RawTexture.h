#pragma once

#include <string>
#include <stb_image.h>

class RawTexture {
private:
    int width = 0;
    int height = 0;
    int channels = 0;
    void* data = nullptr;
    std::string filename;
public:
    RawTexture(const std::string& filename) :
        filename(filename)
    {}

    ~RawTexture() {
		stbi_image_free(data);
    }

    void import()
    {
        stbi_set_flip_vertically_on_load(1);
        data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    }

    const int getWidth() const { return width; }
    const int getHeight() const { return height; }
    const int getChannels() const { return channels; }
    void* getData() const { return data; }
};