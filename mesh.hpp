#pragma once

#include <iostream>
#include <vector>
#include <map>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

class Mesh
{
public:
    Mesh();

public:
    std::vector<glm::dvec4> &getVertices()
    {
        return vertices_;
    }

    const std::vector<int> &getIndices() const
    {
        return indices_;
    }

    const std::vector<glm::dvec3>& getNormals() const
    {
        return normals_;
    }

    const std::vector<glm::dvec3>& getColors() const
    {
        return colors_;
    }

    int addVertex(const glm::dvec3& v)
    {
        return addVertex(glm::dvec4(v, 1.0));
    }

    void addTriangle(
        int a, int b, int c, const glm::dvec3& color);

    void clip(const glm::dmat4& projection, const glm::dmat4& viewport);

    Mesh& operator*=(const glm::dmat4& rhs)
    {
        for (auto& v : vertices_)
            v = rhs * v;
        glm::dmat3 m = rhs;
        for (auto& n : normals_)
            n = m * n;
        return *this;
    }
    
    friend Mesh operator*(const glm::dmat4& lhs, Mesh rhs)
    {
        rhs *= lhs;
        return rhs;
    }

    void clear()
    {
        vertex_lookup_.clear();
        vertices_.clear();
        indices_.clear();
        normals_.clear();
        colors_.clear();
    }

private:
    int addVertex(const glm::dvec4& v);

    void addTriangle(
        int a, int b, int c, const glm::dvec3& color, const glm::dvec3& normal);

private:
    std::map<double, std::map<double, std::map<double, int>>> vertex_lookup_;
    std::vector<glm::dvec4> vertices_;
    std::vector<int> indices_;
    std::vector<glm::dvec3> normals_;
    std::vector<glm::dvec3> colors_;
};

