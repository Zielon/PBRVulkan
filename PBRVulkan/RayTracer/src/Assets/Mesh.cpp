#include "Mesh.h"

#include <tiny_obj_loader.h>

#include <unordered_map>

namespace Assets
{
	Mesh::Mesh(const std::string& path)
	{
		loader = std::async(std::launch::async, [this, path]()
		{
			Load(path);
		});
	}

	void Mesh::Wait()
	{
		if (loader.valid())
			loader.get();
	}

	void Mesh::Load(const std::string& path)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
		{
			throw std::runtime_error(warn + err);
		}

		std::unordered_map<Geometry::Vertex, uint32_t> uniqueVertices{};

#pragma omp parallel for collapse(2)
		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Geometry::Vertex vertex{};

				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				if (!attrib.normals.empty())
				{
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}

				if (!attrib.texcoords.empty() && index.texcoord_index >= 0)
				{
					vertex.texCoords = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}

				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}
}
