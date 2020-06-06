#pragma once

#include "Graphics/Renderer/MainRenderTarget.h"
#include <GL/glew.h>

class OpenGLMainRenderTarget : public MainRenderTarget {
public:
    virtual void bind(RenderContext* renderContext) override {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};