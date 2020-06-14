#pragma once

#include "Graphics/Renderer/Texture2D.h"
#include "OpenGLUtils.h"

class OpenGLRenderTarget;

class OpenGLTexture2D : public Texture2D {
private:
	friend class OpenGLRenderTarget;

    GLenum m_InternalFormat;
	GLenum m_DataFormat;
	public:
	GLuint textureId;
public:
    OpenGLTexture2D(int width, int height, int location, void* data, TextureFormat textureFormat, int numMips) :
        Texture2D(width, height, location, textureFormat, numMips) 
    {
		m_InternalFormat = getTextureFormat(textureFormat);
		m_DataFormat = getInternalTextureFormat(textureFormat);

		/*glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, width, height, 0, m_DataFormat, getInternalType(textureFormat), nullptr);*/

		glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
		glTextureStorage2D(textureId, numMips + 1, m_InternalFormat, width, height);

		glTextureParameteri(textureId, GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(textureId, GL_TEXTURE_MAX_LEVEL, numMips);

		if (data != nullptr) {
	        setData(data);
		}
    }

	virtual void setData(void* data) override {
		glTextureSubImage2D(textureId, 0, 0, 0, getWidth(), getHeight(), m_DataFormat, getInternalType(textureFormat), data);
	}

	virtual void generateMips() override {
		glGenerateTextureMipmap(textureId);
	}

	~OpenGLTexture2D() {
		glDeleteTextures(1, &textureId);
	}

    void bind(RenderContext* context) {
		glActiveTexture(GL_TEXTURE0 + getLocation());
		glBindTexture(GL_TEXTURE_2D, textureId);
	}

	virtual void bindToUnit(const int location, RenderContext* renderContext) override {
		glActiveTexture(GL_TEXTURE0 + location);
		glBindTexture(GL_TEXTURE_2D, textureId);
	}
};