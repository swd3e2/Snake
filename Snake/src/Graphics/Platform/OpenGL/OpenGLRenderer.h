#pragma once

#include "Graphics/Renderer.h"
#include <GLFW/glfw3.h>
#include "OpenGLContext.h"
#include "OpenGLMainRenderTarget.h"

class OpenGlRenderer : public Renderer {
public:
    OpenGlRenderer() {
        _rendererType = RendererType::OpenGL;
        _renderContext = new OpenGLContext();
        _mainRenderTarget = new OpenGLMainRenderTarget();
    }

    virtual void drawIndexed(int cnt) {
        glDrawElements(GL_TRIANGLES, cnt, GL_UNSIGNED_INT, 0);
    }
};