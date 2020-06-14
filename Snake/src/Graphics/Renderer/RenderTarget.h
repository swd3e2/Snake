#pragma once

#include "CommonTypes.h";
#include "Graphics/Bindable.h"
#include "Texture2D.h"
#include <memory>

class RenderTarget : public Bindable {
public:
    virtual ~RenderTarget() {}
    virtual void clear(RenderContext* renderContext) = 0;

    virtual void setColorTexture(const std::shared_ptr<Texture2D>& texture, int slot, int level = 0) = 0;
    virtual void removeColorTexture(int slot, int level) = 0;
    virtual void setDepthTexture(const std::shared_ptr<Texture2D>& texture, int level = 0) = 0;

    static RenderTarget* create();
};