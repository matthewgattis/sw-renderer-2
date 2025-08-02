#pragma once

#include <iostream>
#include <vector>
#include <map>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

class Mesh
{
public:
    Mesh() = delete;
    Mesh(
        const std::vector<glm::dvec3> &vertices,
        const std::vector<glm::ivec3> &indices);

    const std::vector<glm::dvec4> &getVertices() { return vertices_; }
    const std::vector<glm::ivec3> &getIndices() { return indices_; }
    const std::vector<glm::dvec3> &getNormals() { return normals_; }

public:
    Mesh& operator*=(const glm::dmat4 &m);

private:
    std::vector<glm::dvec4> vertices_;
    std::vector<glm::ivec3> indices_;
    std::vector<glm::dvec3> normals_;
};

