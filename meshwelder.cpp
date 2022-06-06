#include "mesh.hpp"

#include <exception>

#define LOG_MODULE_NAME ("Mesh")
#include "log.hpp"

Mesh::Mesh()
{
}

const std::vector<glm::dvec4> &Mesh::getVertices() const
{
    return vertices_;
}

const std::vector<int> &Mesh::getIndices() const
{
    return indices_;
}

const std::vector<glm::dvec3> &Mesh::getNormals() const
{
    return normals_;
}

const std::vector<glm::dvec3>& Mesh::getColors() const
{
    return colors_;
}

int Mesh::addVertex(const glm::dvec3 &v)
{
    try
    {
        return vertex_lookup_.at(v.x).at(v.y).at(v.z);
    }
    catch (std::out_of_range &e)
    {
        /*
#ifndef NDEBUG
        LOG_DEBUG << "Vertex out of range. (" << e.what() << ")" << std::endl;
#endif
		*/
    }

    vertices_.push_back(glm::dvec4(v, 1.0));
    vertex_lookup_[v.x][v.y][v.z] = vertices_.size() - 1;
    return vertices_.size() - 1;
}

void Mesh::addTriangle(int a, int b, int c, const glm::dvec3& color)
{
    glm::dvec3 normal(
        glm::normalize(glm::cross(
			glm::dvec3(vertices_[b] - vertices_[a]),
			glm::dvec3(vertices_[c] - vertices_[a]))));

    addTriangle(a, b, c, color, normal);
}

void Mesh::addTriangle(
    int a, int b, int c, const glm::dvec3& color, const glm::dvec3& normal)
{
    indices_.push_back(a);
    indices_.push_back(b);
    indices_.push_back(c);

    normals_.push_back(normal);
    colors_.push_back(color);
}

void Mesh::transformVertices(const glm::dmat4& m)
{
    for (auto& v : vertices_)
        v = m * v;
}

void Mesh::transformNormals(const glm::dmat3& m)
{
    for (auto& n : normals_)
        n = m * n;
}

