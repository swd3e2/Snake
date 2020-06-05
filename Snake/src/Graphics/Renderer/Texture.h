#pragma once

#include <string>
#include <stb_image.h>
#include <GL/glew.h>
#include "Graphics/Bindable.h"

class Texture : public Bindable {
public:
    int m_Width = 0;
    int m_Height = 0;

	GLenum m_InternalFormat;
	GLenum m_DataFormat;
	GLuint textureId;
public:
	Texture(std::string path) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        m_Width = width;
        m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4) {
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		} else if (channels == 3) {
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
		glTextureStorage2D(textureId, 1, internalFormat, m_Width, m_Height);

		glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


		glTextureSubImage2D(textureId, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	void bind(int location) {
		glActiveTexture(GL_TEXTURE0 + location);
		glBindTexture(GL_TEXTURE_2D, textureId);
	}

	void bind(RenderContext* context) {
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, textureId);
	}

	const int getWidth() const { return m_Width; }
	const int getHeight() const { return m_Height; }
};