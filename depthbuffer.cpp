#include "depthbuffer.hpp"

#include <limits>
#include <ranges>
#include <algorithm>

DepthBuffer::DepthBuffer(int width, int height)
    : depth_(width * height, std::numeric_limits<double>::max())
{
}

void DepthBuffer::clear()
{
    std::ranges::fill(depth_, std::numeric_limits<double>::max());
}
