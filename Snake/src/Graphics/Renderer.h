#pragma once

#include <memory>
#include "RendererType.h"
#include <assert.h>
#include "Renderer/RenderContext.h"
#include "Bindable.h"
#include "Renderer/MainRenderTarget.h"

class Renderer {
private:
    static Renderer* _instance;
protected:
    static RenderContext* _renderContext;
    static MainRenderTarget* _mainRenderTarget;
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
    static RendererType getType() { return _rendererType; };
    static MainRenderTarget* getMainRenderTarget() { return _mainRenderTarget; }
};