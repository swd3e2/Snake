#pragma once

#include "Graphics/Renderer/RenderTarget.h"
#include "Graphics/Bindable.h"
#include <GL/glew.h>
#include <vector>
#include <iostream>
#include "Graphics/Renderer/CommonTypes.h"
#include "Graphics/Renderer/Texture2D.h"
#include "OpenGLTexture2D.h"
#include <unordered_map>

class OpenGLRenderTarget : public RenderTarget
{
public:
    unsigned int fbo;
	std::unordered_map<int, std::shared_ptr<Texture2D>> colorTexturesMap;
	std::shared_ptr<Texture2D> depthTexture;
public:
    OpenGLRenderTarget() {
        glCreateFramebuffers(1, &fbo);
		glNamedFramebufferDrawBuffer(fbo, GL_NONE);
		glNamedFramebufferReadBuffer(fbo, GL_NONE);
    }

	virtual void setColorTexture(const std::shared_ptr<Texture2D>& texture, int slot, int level) override {
		GLuint tex = std::static_pointer_cast<OpenGLTexture2D>(texture)->textureId;
		glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0 + slot, tex, level);

		colorTexturesMap[slot] = texture;
		this->activateDrawBuffers();
		checkErrors();
	}

	virtual void removeColorTexture(int slot, int level) override {
		glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0 + slot, 0, level);

		colorTexturesMap.erase(slot);
		this->activateDrawBuffers();
		checkErrors();
	}

	virtual void setDepthTexture(const std::shared_ptr<Texture2D>& texture, int level) override {
		depthTexture = texture;

		GLuint target = 0;
		if (texture->getFormat() == TextureFormat::D24S8 || texture->getFormat() == TextureFormat::D32_S8X24) {
			target = GL_DEPTH_STENCIL_ATTACHMENT;
		} else {
			target = GL_DEPTH_ATTACHMENT;
		}

		glNamedFramebufferTexture(fbo, target, std::static_pointer_cast<OpenGLTexture2D>(texture)->textureId, level);

		checkErrors();
	}

	virtual ~OpenGLRenderTarget() {
		glDeleteFramebuffers(1, &fbo);
	}

    virtual void bind(RenderContext* renderContext) override {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }

	virtual void clear(RenderContext* renderContext) override {
		GLint prevFbo = 0;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, prevFbo);
	}
private:
	void activateDrawBuffers() {
		const int totatlDrawBuffersCount = colorTexturesMap.size();
		std::vector<GLuint> temp;
		temp.reserve(totatlDrawBuffersCount);

		for (auto& it : colorTexturesMap) {
			temp.push_back(GL_COLOR_ATTACHMENT0 + it.first);
		}

		glNamedFramebufferDrawBuffers(fbo, totatlDrawBuffersCount, temp.data());
	}

	void checkErrors() {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "Error occured while creating framebuffer" << std::endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};