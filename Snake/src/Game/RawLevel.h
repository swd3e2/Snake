#pragma once

#include <vector>

struct TileIndex {
    int x = 0;
    int y = 0;

    TileIndex(int x, int y) {
        this->x = x;
        this->y = y;
    }
};

struct TileData {
    bool flipX = false;
    bool flipY = false;
    int tileIndex = 0;
};

struct RawLevel {
    std::vector<std::vector<TileData>> map;
};