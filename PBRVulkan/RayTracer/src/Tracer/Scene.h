#pragma once

#include <memory>

#include "../Geometry/Vertex.h"
#include "../Vulkan/Vulkan.h"
#include "../Vulkan/Buffer.hpp"

namespace Tracer
{
	class Scene final
	{
	public:
		NON_COPIABLE(Scene)

		Scene(const class Vulkan::Device& device, const class Vulkan::CommandBuffers& commandBuffers);
		~Scene();

		[[nodiscard]] const Vulkan::Buffer<Geometry::Vertex>& GetVertexBuffer() const
		{
			return *vertexBuffer;
		}

		[[nodiscard]] const Vulkan::Buffer<Geometry::Vertex>& GetIndexBuffer() const
		{
			return *indexBuffer;
		}

	private:
		const class Vulkan::Device& device;
		const class Vulkan::CommandBuffers& commandBuffers;
		std::unique_ptr<Vulkan::Buffer<Geometry::Vertex>> vertexBuffer;
		std::unique_ptr<Vulkan::Buffer<Geometry::Vertex>> indexBuffer;

		void CreateBuffers();
	};
}
