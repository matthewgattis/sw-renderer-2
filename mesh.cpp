#include "mesh.hpp"

Mesh::Mesh(
    const std::vector<glm::dvec3> &vertices,
    const std::vector<glm::ivec3> &indices) :
        indices_(indices)
{
    vertices_.reserve(vertices.size());
    for (const auto &v : vertices)
        vertices_.emplace_back(v, 1.0);
    
    normals_.reserve(indices_.size() / 3);
    for (const auto &index : indices_)
    {
        const glm::dvec3 &a = vertices_[index[0]];
        const glm::dvec3 &b = vertices_[index[1]];
        const glm::dvec3 &c = vertices_[index[2]];

        glm::dvec3 n = glm::normalize(glm::cross(b - a, c - a));
        normals_.push_back(n);
    }
}

Mesh& Mesh::operator*=(const glm::dmat4 &m)
{
    for (auto &v : vertices_)
        v = m * v;

    // Good enough for uniform scaling.
    for (auto &n : normals_)
        n = glm::normalize(glm::dvec3(m * glm::dvec4(n, 0.0)));

    return *this;
}

