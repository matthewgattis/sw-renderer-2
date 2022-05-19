#pragma once

#include <memory>

#include <glm/glm.hpp>

class Framebuffer;

class Line
{
public:
    Line(
        const std::shared_ptr<Framebuffer> &framebuffer);

    void draw(
        const glm::ivec2 &p1,
        const glm::ivec2 &p2,
        const glm::dvec3 &c);

private:
    std::shared_ptr<Framebuffer> framebuffer_;
};

