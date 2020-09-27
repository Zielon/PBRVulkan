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

		[[nodiscard]] const std::vector<Geometry::Vertex>& GetVertices() const
		{
			return vertices;
		}

		[[nodiscard]] const std::vector<uint32_t>& GetIndecies() const
		{
			return indices;
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
			: transform(transformation), materialId(matId), meshId(meshId) { }

		~MeshInstance() = default;

		glm::mat4 transform;
		int materialId;
		int meshId;
	};
}
