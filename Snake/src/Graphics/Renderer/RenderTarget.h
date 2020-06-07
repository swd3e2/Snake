#pragma once

#include "CommonTypes.h";
#include "Graphics/Bindable.h"

class RenderTarget : public Bindable {
private:
    int width = 0;
    int height = 0;
    int cnt = 0;
    TextureFormat colorTextureFormat;
public:
    RenderTarget(int width, int height, int cnt, TextureFormat colorTextureFormat) :
        cnt(cnt), width(width), height(height), colorTextureFormat(colorTextureFormat)
    {}
    virtual ~RenderTarget() {}
    const int getColorTextureCount() const { return cnt; }
    virtual void clear(RenderContext* renderContext) = 0;
    static RenderTarget* create(int width, int height, int cnt, TextureFormat colorTextureFormat = TextureFormat::RGBA8);
};