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
    OpenGLTexture2D(int width, int height, int location, void* data, TextureFormat textureFormat = TextureFormat::RGBA8) :
        Texture2D(width, height, location, textureFormat) 
    {
		glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
		glTextureStorage2D(textureId, 1, getTextureFormat(textureFormat), width, height);

		glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_REPEAT);

        if (data != nullptr) {
            glTextureSubImage2D(textureId, 0, 0, 0, width, height, getInternalTextureFormat(textureFormat), GL_UNSIGNED_BYTE, data);
        }
    }

	~OpenGLTexture2D() {
		glDeleteTextures(1, &textureId);
	}

    void bind(RenderContext* context) {
		glActiveTexture(GL_TEXTURE0 + getLocation());
		glBindTexture(GL_TEXTURE_2D, textureId);
	}
};