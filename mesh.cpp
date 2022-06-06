#include "mesh.hpp"

#include <exception>

#define LOG_MODULE_NAME ("Mesh")
#include "log.hpp"

Mesh::Mesh()
{
}

int Mesh::addVertex(const glm::dvec4 &v)
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

    vertices_.push_back(v);
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

void Mesh::clip(const glm::dmat4& projection, const glm::dmat4& viewport)
{
	for (auto& v : this->vertices_)
		v = projection * v;

	Mesh other;
	std::vector<Mesh*> meshes
	{
		this,
		&other
    };

	Mesh* next_mesh;
    for (int i = 0; i < 6; i++)
    {
        Mesh* current_mesh = meshes[i % 2];
        next_mesh = meshes[(i + 1) % 2];
        
		double sign = i < 3 ? -1.0 : 1.0;
		int component = i % 3;
        int triangle_count = current_mesh->indices_.size() / 3;;

		std::vector<int> out;
		std::vector<int> in;

        for (int j = 0; j < triangle_count; j++)
        {
			int index1 = 3 * j;

			if (i == 0)
			{
				glm::dvec3 a(current_mesh->vertices_[current_mesh->indices_[index1 + 0]]);
				glm::dvec3 b(current_mesh->vertices_[current_mesh->indices_[index1 + 1]]);
				glm::dvec3 c(current_mesh->vertices_[current_mesh->indices_[index1 + 2]]);
				if (glm::cross(b - a, c - a).z < 0.0)
					continue;
			}

			for (int k = 0; k < 3; k++)
			{
				int index2 = current_mesh->indices_[index1 + k];
				const auto& v = current_mesh->vertices_[index2];
				if (sign * v[component] > v.w)
					out.push_back(index2);
				else
					in.push_back(index2);
			}

			const int in_count = in.size();
            if (in_count > 0)
            {
				const auto color = current_mesh->colors_[j];
				const auto normal = current_mesh->normals_[j];
                
                if (in_count == 1)
                {
					const auto& a = current_mesh->vertices_[in[0]];
					const auto& b = current_mesh->vertices_[out[0]];
					const auto& c = current_mesh->vertices_[out[1]];
					glm::dvec4 d;
					glm::dvec4 e;
					if (sign > 0.0)
					{
						d = a + ((a.w - a[component]) / (a.w - b.w - a[component] + b[component])) * (b - a);
						e = a + ((a.w - a[component]) / (a.w - c.w - a[component] + c[component])) * (c - a);
					}
					else
					{
						d = a + ((a.w + a[component]) / (a.w - b.w + a[component] - b[component])) * (b - a);
						e = a + ((a.w + a[component]) / (a.w - c.w + a[component] - c[component])) * (c - a);
					}
					next_mesh->addTriangle(
						next_mesh->addVertex(a),
						next_mesh->addVertex(d),
						next_mesh->addVertex(e),
						color,
						normal);
                }
				else if (in_count == 2)
				{
					const auto& a = current_mesh->vertices_[in[0]];
					const auto& b = current_mesh->vertices_[in[1]];
					const auto& c = current_mesh->vertices_[out[0]];
					glm::dvec4 d;
					glm::dvec4 e;
					if (sign > 0.0)
					{
						d = a + ((a.w - a[component]) / (a.w - c.w - a[component] + c[component])) * (c - a);
						e = b + ((b.w - b[component]) / (b.w - c.w - b[component] + c[component])) * (c - b);
					}
					else
					{
						d = a + ((a.w + a[component]) / (a.w - c.w + a[component] - c[component])) * (c - a);
						e = b + ((b.w + b[component]) / (b.w - c.w + b[component] - c[component])) * (c - b);
					}
					next_mesh->addTriangle(
						next_mesh->addVertex(a),
						next_mesh->addVertex(b),
						next_mesh->addVertex(d),
						color,
						normal);
					next_mesh->addTriangle(
						next_mesh->addVertex(b),
						next_mesh->addVertex(e),
						next_mesh->addVertex(d),
						color,
						normal);
				}
				else
				{
					next_mesh->addTriangle(
						next_mesh->addVertex(current_mesh->vertices_[in[0]]),
						next_mesh->addVertex(current_mesh->vertices_[in[1]]),
						next_mesh->addVertex(current_mesh->vertices_[in[2]]),
						color,
						normal);
				}
            }
			in.clear();
			out.clear();
        }
		current_mesh->clear();
    }

	for (auto& v : next_mesh->vertices_)
		v = viewport * (v / v.w);
}

