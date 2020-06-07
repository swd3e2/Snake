#pragma once

#include "CommonTypes.h";
#include "Graphics/Bindable.h"
#include "Texture2D.h"
#include <memory>

class RenderTarget : public Bindable {
public:
    virtual ~RenderTarget() {}
    virtual void clear(RenderContext* renderContext) = 0;

    virtual void addColorTexture(const std::shared_ptr<Texture2D>& texture) = 0;
    virtual void setDepthTexture(const std::shared_ptr<Texture2D>& texture) = 0;

    static RenderTarget* create();
};