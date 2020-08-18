#pragma once

#include <memory>
#include "RendererType.h"
#include <assert.h>
#include "Renderer/RenderContext.h"
#include "Bindable.h"
#include "Renderer/MainRenderTarget.h"
#include "Window.h"
#include "Renderer/CommonTypes.h"
#include "Viewport.h"
#include "EventSystem/EventSystem.h"

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

    inline RenderContext* getContext() { return _renderContext; }

    void bind(Bindable* bindable) {
        bindable->bind(_renderContext);
    }

    virtual void drawIndexed(int cnt) = 0;
    virtual void draw(int cnt) = 0;
    
    void bindMainRenderTarget() {
        _mainRenderTarget->bind(_renderContext);
    }

	virtual Window* createWindow(int width, int height, EventSystem* eventSystem) = 0;
	virtual void setViewport(int x0, int y0, int x1, int y1) = 0;
	virtual void setViewport(const Viewport& viewport) = 0;
    virtual void swapBuffers() = 0;
    virtual void unbindResource(int slot) = 0;
    virtual void setPrimitiveTopology(PrimitiveTopology topology) = 0;

    static Renderer* create(RendererType type);

    static Renderer* instance() { return _instance; }
    static RendererType getType() { return _rendererType; };
    static MainRenderTarget* getMainRenderTarget() { return _mainRenderTarget; }
};