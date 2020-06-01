#include "Graphics/Renderer.h"
#include <GLFW/glfw3.h>
#include "OpenGLContext.h"

#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

class OpenGlRenderer : public Renderer {
private:
    OpenGLContext* context;
public:
    OpenGlRenderer() {
        _rendererType = RendererType::OpenGL;
        context = new OpenGLContext();
    }
    
    virtual void bindIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override {
        indexBuffer->bind(context);
    }

    virtual void bindVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override {
        vertexBuffer->bind(context);
    }

    virtual void bindRenderBuffer(const std::shared_ptr<RenderTarget>& renderTarget) override {
        
    }

    virtual void bindDepthBuffer(const std::shared_ptr<DepthBuffer>& depthBuffer) override { 
        
    }

    virtual void bindConstantBuffer(const std::shared_ptr<ConstantBuffer>& constantBuffer) override {
        constantBuffer->bind(context);
    }

    virtual void bindShaderPipeline(const std::shared_ptr<ShaderPipeline>& shaderPipeline) override {
        shaderPipeline->bind(context);
    }

    virtual void bindInputLayout(const std::shared_ptr<ShaderInputLayout>& inputLayout) override {

    }

    virtual void drawIndexed(int cnt) {
        glDrawElements(GL_TRIANGLES, cnt, GL_UNSIGNED_INT, 0);
    }
};

#endif