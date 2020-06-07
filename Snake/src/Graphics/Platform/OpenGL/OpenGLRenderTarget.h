#pragma once

#include "Graphics/Renderer/RenderTarget.h"
#include "Graphics/Bindable.h"
#include <GL/glew.h>
#include <vector>
#include <iostream>
#include "Graphics/Renderer/CommonTypes.h"

class OpenGLRenderTarget : public RenderTarget
{
public:
    unsigned int fbo;
    unsigned int* colorTexture;
    unsigned int depthTexture;
public:
    OpenGLRenderTarget(int width, int height, int cnt, TextureFormat colorTextureFormat = TextureFormat::RGBA8) :
        RenderTarget(width, height, cnt, colorTextureFormat)
    {
        colorTexture = new unsigned int[cnt];

        glCreateFramebuffers(1, &fbo);
		glCreateTextures(GL_TEXTURE_2D, cnt, colorTexture);

		std::vector<unsigned int> attachmentsToActivate;
		for (int i = 0; i < cnt; i++) {
			glTextureStorage2D(colorTexture[i], 1, getTextureFormat(colorTextureFormat), width, height);
			glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0 + i, colorTexture[i], 0);
			attachmentsToActivate.push_back(GL_COLOR_ATTACHMENT0 + i);
		}

		unsigned int temp = GL_COLOR_ATTACHMENT0;
		glNamedFramebufferDrawBuffers(fbo, cnt, &temp);

		glCreateTextures(GL_TEXTURE_2D, 1, &depthTexture);
		glTextureStorage2D(depthTexture, 1, GL_DEPTH24_STENCIL8, width, height);

		glNamedFramebufferTexture(fbo, GL_DEPTH_STENCIL_ATTACHMENT, depthTexture, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "Error occured while creating framebuffer" << std::endl;
		}

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

	virtual ~OpenGLRenderTarget() {
		glDeleteTextures(getColorTextureCount(), colorTexture);
		glDeleteTextures(1, &depthTexture);
		glDeleteFramebuffers(1, &fbo);
	}

    virtual void bind(RenderContext* renderContext) override {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }

	virtual void clear(RenderContext* renderContext) override {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(1,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
private:
    unsigned int getTextureFormat(TextureFormat format)
	{
		switch (format) {
			case TextureFormat::RGBA8:     return GL_RGBA8;
			case TextureFormat::RGBA32:    return GL_RGBA32F;
		}
		return GL_RGBA;
	}
};