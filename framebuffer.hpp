#pragma once

#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

class Framebuffer
{
public:
    virtual void setPixelColor(
        const glm::ivec2 &p,
        const glm::dvec3 &c) = 0;

    virtual double getPixelDepth(
        const glm::ivec2 &p) const = 0;

    virtual void setPixelDepth(
        const glm::ivec2 &p,
        double d) = 0;

    virtual void clearColorBuffer(
        const glm::dvec3 &c) = 0;

    virtual void clearDepthBuffer(
        double d) = 0;

protected:
    Framebuffer()
    {
    }
};

