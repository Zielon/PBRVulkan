#pragma once

#include <string>
#include <vector>

#include "../Geometry/Vertex.h"

namespace Assets
{
	class Mesh
	{
	public:
		Mesh(const std::string& path);

		[[nodiscard]] std::vector<Geometry::Vertex>& GetVertices()
		{
			return vertices;
		}

		[[nodiscard]] uint32_t GetVerticesSize() const
		{
			return vertices.size();
		}

		[[nodiscard]] const std::vector<uint32_t>& GetIndecies() const
		{
			return indices;
		}

		[[nodiscard]] uint32_t GetIndeciesSize() const
		{
			return indices.size();
		}

	private:
		std::vector<Geometry::Vertex> vertices;
		std::vector<uint32_t> indices;

		void Load(const std::string& path);
	};

	class MeshInstance
	{
	public:
		MeshInstance(int meshId, glm::mat4 transformation, int matId)
			: modelTransform(transformation), materialId(matId), meshId(meshId) { }

		~MeshInstance() = default;

		glm::mat4 modelTransform = glm::mat4(1.f);
		int materialId;
		int meshId;
	};
}
