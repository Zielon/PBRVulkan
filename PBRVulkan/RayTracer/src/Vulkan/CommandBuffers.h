#pragma once

#include <vector>

#include "Vulkan.h"

namespace Vulkan
{
	/**
	 * Class to record all operations in a buffer for multiple threads.
	 * [] operators provides access to command buffers.
	 */
	class CommandBuffers final
	{
	public:
		NON_COPIABLE(CommandBuffers)

		CommandBuffers(const class Device& device, uint32_t size);
		~CommandBuffers();

		[[nodiscard]] VkCommandPool GetPool() const { return commandPool; }
		[[nodiscard]] uint32_t Size() const { return static_cast<uint32_t>(commandBuffers.size()); }
		const VkCommandBuffer& operator [](const size_t i) const { return commandBuffers[i]; }
		VkCommandBuffer Begin(size_t i);
		void End(size_t);

	private:
		const Device& device;
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;
	};
}
