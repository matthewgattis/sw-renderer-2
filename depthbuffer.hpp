#pragma once

#include <vector>
#include <span>

class DepthBuffer
{
public:
    DepthBuffer() = delete;
    DepthBuffer(int width, int height);

    std::span<double> get() { return std::span<double>{ depth_ }; }

    void clear();

private:
    std::vector<double> depth_;
};
