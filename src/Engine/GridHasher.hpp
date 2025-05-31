#include <iostream>

struct GridHasher {
    float cellSize;

    GridHasher(float cellSize)
        : cellSize(cellSize)
        {}

    int64_t Hash(int32_t x, int32_t y) const {
        return ((int64_t)x << 32) | y;
    }

    int32_t GridCoord(float value) const {
        return (int32_t)(value / cellSize);
    }
};
