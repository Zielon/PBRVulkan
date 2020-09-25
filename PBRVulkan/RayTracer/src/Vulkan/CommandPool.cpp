#include "CommandPool.h"

#include "Device.h"

namespace Vulkan
{
	CommandPool::CommandPool(const Device& device): device(device)
	{
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = device.GraphicsFamilyIndex;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VK_CHECK(vkCreateCommandPool(device.Get(), &poolInfo, nullptr, &commandPool),
		         "Create command pool");
	}

	CommandPool::~CommandPool()
	{
		if (commandPool != nullptr)
		{
			vkDestroyCommandPool(device.Get(), commandPool, nullptr);
			commandPool = nullptr;
		}
	}
}
