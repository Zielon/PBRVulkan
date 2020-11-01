#include "Mesh.h"

#include <tiny_obj_loader.h>

#include <unordered_map>

namespace Assets
{
	Mesh::Mesh(const std::string& path)
	{
		Load(path);
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

		for (const auto& shape : shapes)
		{
			// Loop over faces(polygon)
			size_t index_offset = 0;

			for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
			{
				int fv = shape.mesh.num_face_vertices[f];

				for (size_t v = 0; v < fv; v++)
				{
					tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
					tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
					tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
					tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
					tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
					tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
					tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];

					tinyobj::real_t tx, ty;

					if (!attrib.texcoords.empty() && idx.texcoord_index >= 0)
					{
						tx = attrib.texcoords[2 * idx.texcoord_index + 0];
						ty = attrib.texcoords[2 * idx.texcoord_index + 1];
					}
					else
					{
						tx = ty = 0.f;
					}

					Geometry::Vertex vertex{};
					vertex.position = { vx, vy, vz };
					vertex.normal = { nx, ny, nz };
					vertex.texCoords = { tx, ty };

					if (uniqueVertices.count(vertex) == 0)
					{
						uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
						vertices.push_back(vertex);
					}

					indices.push_back(uniqueVertices[vertex]);
				}

				index_offset += fv;
			}
		}
	}
}
