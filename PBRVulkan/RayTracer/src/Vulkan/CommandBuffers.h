#pragma once

#include <vector>

#include "Vulkan.h"

namespace Vulkan
{
	class CommandBuffers final
	{
	public:
		NON_COPIABLE(CommandBuffers)

		CommandBuffers(const class Device& device, uint32_t size);
		~CommandBuffers();

		VkCommandPool Get() const { return commandPool; }

		uint32_t Size() const { return static_cast<uint32_t>(commandBuffers.size()); }
		VkCommandBuffer& operator [](const size_t i) { return commandBuffers[i]; }

		VkCommandBuffer Begin(size_t i);
		void End(size_t);

	private:
		const Device& device;
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;
	};
}
