#pragma once

#include <vector>

#include "Vulkan.h"

namespace Vulkan
{
	/**
	 * Class to record all operations in a buffer for multiple threads.
	 * All drawing operations are added throughout the pipeline to command buffers.
	 * At the final presentation step commands are sent to queue for final processing.
	 * [] operators provides access to command buffers.
	 */
	class CommandBuffers final
	{
	public:
		NON_COPIABLE(CommandBuffers)

		CommandBuffers(const class Device& device, uint32_t size);
		~CommandBuffers();

		[[nodiscard]] VkCommandPool GetPool() const
		{
			return commandPool;
		}

		[[nodiscard]] uint32_t Size() const
		{
			return static_cast<uint32_t>(commandBuffers.size());
		}

		const VkCommandBuffer& operator [](const size_t i) const
		{
			return commandBuffers[i];
		}

		[[nodiscard]] const class Device& GetDevice() const
		{
			return device;
		}

		VkCommandBuffer Begin(size_t i);
		void End(size_t);

	private:
		const Device& device;
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;
	};
}
