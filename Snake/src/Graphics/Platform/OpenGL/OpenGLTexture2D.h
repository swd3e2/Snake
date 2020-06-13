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

		glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
		if (textureFormat != TextureFormat::D32) {
			glTextureStorage2D(textureId, numMips + 1, m_InternalFormat, width, height);
			glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_REPEAT);
			//glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			//glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTextureParameteri(textureId, GL_TEXTURE_BASE_LEVEL, 0);
			glTextureParameteri(textureId, GL_TEXTURE_MAX_LEVEL, numMips); 
			glTextureParameteri(textureId, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		} else {
			glBindTexture(GL_TEXTURE_2D, textureId);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}

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
};