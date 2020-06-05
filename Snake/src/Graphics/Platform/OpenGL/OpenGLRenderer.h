#include "Graphics/Renderer.h"
#include <GLFW/glfw3.h>
#include "OpenGLContext.h"

#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

class OpenGlRenderer : public Renderer {
public:
    OpenGlRenderer() {
        _rendererType = RendererType::OpenGL;
        _renderContext = new OpenGLContext();
    }

    virtual void drawIndexed(int cnt) {
        glDrawElements(GL_TRIANGLES, cnt, GL_UNSIGNED_INT, 0);
    }
};

#endif