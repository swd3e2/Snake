#pragma once

#include "Graphics/Renderer/RenderTarget.h"
#include "Graphics/Bindable.h"
#include <GL/glew.h>
#include <vector>
#include <iostream>
#include "Graphics/Renderer/CommonTypes.h"
#include "Graphics/Renderer/Texture2D.h"
#include "OpenGLTexture2D.h"

class OpenGLRenderTarget : public RenderTarget
{
public:
    unsigned int fbo;
	unsigned int colorTexturesCount = 0;
	std::vector<std::shared_ptr<Texture2D>> colorTextures;
	std::shared_ptr<Texture2D> depthTexture;
public:
    OpenGLRenderTarget() {
        glCreateFramebuffers(1, &fbo);
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

	virtual void addColorTexture(const std::shared_ptr<Texture2D>& texture) override {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		
		GLuint tex = std::static_pointer_cast<OpenGLTexture2D>(texture)->textureId;
		glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0 + colorTexturesCount, tex, 0);

		unsigned int* temp = new unsigned int[colorTexturesCount + 1];
		for (int i = 0; i < colorTexturesCount + 1; i++) 
			temp[i] = GL_COLOR_ATTACHMENT0 + i;

		glNamedFramebufferDrawBuffers(fbo, colorTexturesCount + 1, temp);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "Error occured while creating framebuffer" << std::endl;
		}
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

		colorTextures.push_back(texture);
		colorTexturesCount++;
	}

	virtual void setDepthTexture(const std::shared_ptr<Texture2D>& texture) override {
		depthTexture = texture;
		glNamedFramebufferTexture(fbo, GL_DEPTH_STENCIL_ATTACHMENT, std::static_pointer_cast<OpenGLTexture2D>(texture)->textureId, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "Error occured while creating framebuffer" << std::endl;
		}
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	virtual ~OpenGLRenderTarget() {
		glDeleteFramebuffers(1, &fbo);
	}

    virtual void bind(RenderContext* renderContext) override {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }

	virtual void clear(RenderContext* renderContext) override {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};