#pragma once

#include "Renderer/RenderContext.h"

class Bindable {
public:
    virtual void bind(RenderContext* renderContext) = 0;
};