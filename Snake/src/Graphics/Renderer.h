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
#include "Renderer/RenderContext.h"
#include "Bindable.h"

class Renderer {
private:
    static Renderer* _instance;
protected:
    static RenderContext* _renderContext;
    static RendererType _rendererType;
public:
    Renderer() {
        if (_instance != nullptr) {
            assert(false);
        }
        _instance = this;
    }

    RenderContext* getContext() { return _renderContext; }

    void bind(Bindable* bindable) {
        bindable->bind(_renderContext);
    }

    virtual void drawIndexed(int cnt) = 0;

    static Renderer* instance() { return _instance; }
    static RendererType getType() {
        return _rendererType;
    };
};