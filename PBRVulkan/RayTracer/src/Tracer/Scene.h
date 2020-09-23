#pragma once

#include <memory>

#include "../Vulkan/Vulkan.h"

namespace Vulkan
{
	class Device;
	class Buffer;
	class CommandBuffers;
}

namespace Tracer
{
	class Scene final
	{
	public:
		NON_COPIABLE(Scene)

		Scene(const class Vulkan::Device& device, const class Vulkan::CommandBuffers& commandBuffers);
		~Scene();

		[[nodiscard]] const class Vulkan::Buffer& GetVertexBuffer() const
		{
			return *vertexBuffer;
		}

		[[nodiscard]] const Vulkan::Buffer& GetIndexBuffer() const
		{
			return *indexBuffer;
		}

	private:
		const class Vulkan::Device& device;
		const class Vulkan::CommandBuffers& commandBuffers;
		std::unique_ptr<class Vulkan::Buffer> vertexBuffer;
		std::unique_ptr<class Vulkan::Buffer> indexBuffer;

		void CreateBuffers();
	};
}
