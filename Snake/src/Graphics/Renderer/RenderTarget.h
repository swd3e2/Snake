#pragma once

class RenderTarget {
private:
    int width = 0;
    int height = 0;
    int cnt = 0;
public:
    RenderTarget(int width, int height, int cnt) :
        cnt(cnt), width(width), height(height)
    {}
};