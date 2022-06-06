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
    const std::vector<glm::dvec4> &getVertices() const;
    const std::vector<int> &getIndices() const;
    const std::vector<glm::dvec3>& getNormals() const;
    const std::vector<glm::dvec3> &getColors() const;

    int addVertex(const glm::dvec3 &v);

    void addTriangle(
        int a, int b, int c, const glm::dvec3& color);

    void addTriangle(
        int a, int b, int c, const glm::dvec3& color, const glm::dvec3& normal);

    void transformVertices(const glm::dmat4& m);

    void transformNormals(const glm::dmat3& m);

private:
    std::map<double, std::map<double, std::map<double, int>>> vertex_lookup_;
    std::vector<glm::dvec4> vertices_;
    std::vector<int> indices_;
    std::vector<glm::dvec3> normals_;
    std::vector<glm::dvec3> colors_;
};

