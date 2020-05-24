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

struct RawLevel {
    std::vector<std::vector<int>> map;
};