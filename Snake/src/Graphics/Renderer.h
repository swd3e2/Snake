#pragma once

#include "Renderer/IndexBuffer.h"
#include "Renderer/ConstantBuffer.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/DepthBuffer.h"
#include "Renderer/ShaderPipeline.h"
#include "Renderer/ShaderInputLayout.h"
#include <memory>
#include "RendererType.h"
#include <assert.h>

class Renderer {
private:
    static Renderer* _instance;
protected:
    static RendererType _rendererType;
public:
    Renderer() {
        if (_instance != nullptr) {
            assert(false);
        }
        _instance = this;
    }

    virtual void bindIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;
    virtual void bindVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;
    virtual void bindRenderBuffer(const std::shared_ptr<RenderTarget>& renderTarget) = 0;
    virtual void bindDepthBuffer(const std::shared_ptr<DepthBuffer>& depthBuffer) = 0;
    virtual void bindShaderPipeline(const std::shared_ptr<ShaderPipeline>& shaderPipeline) = 0;
    virtual void bindInputLayout(const std::shared_ptr<ShaderInputLayout>& inputLayout) = 0;
    virtual void bindConstantBuffer(const std::shared_ptr<ConstantBuffer>& constantBuffer) = 0;
    virtual void drawIndexed(int cnt) = 0;

    static RendererType getType() {
        return _rendererType;
    };
};