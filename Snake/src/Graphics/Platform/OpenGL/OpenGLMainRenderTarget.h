#pragma once

#include "Graphics/Renderer/MainRenderTarget.h"
#include <glad/glad.h>

class OpenGLMainRenderTarget : public MainRenderTarget {
public:
    virtual void bind(RenderContext* renderContext) override {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    virtual void clear(RenderContext* renderContext) override {
		GLint prevFbo = 0;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFbo);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, prevFbo);
    }
};