#pragma once

#include "Vulkan.h"

namespace Vulkan
{
	class CommandPool final
	{
	public:
		NON_COPIABLE(CommandPool)

		CommandPool(const class Device& device, uint32_t familyIndex);
		~CommandPool();

		[[nodiscard]] VkCommandPool Get() const
		{
			return commandPool;
		}

		[[nodiscard]] const Device& GetDevice() const
		{
			return device;
		}

	private:
		const Device& device;
		VkCommandPool commandPool{};
	};
}
