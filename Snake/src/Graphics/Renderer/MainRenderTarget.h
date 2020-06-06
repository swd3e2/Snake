#pragma once

#include "RenderTarget.h"
#include "Graphics/Bindable.h"

class MainRenderTarget : public Bindable {
public:
    virtual void bind(RenderContext* renderContext) = 0;
};