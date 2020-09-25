#include "CommandBuffers.h"

#include "Device.h"
#include "CommandPool.h"

namespace Vulkan
{
	CommandBuffers::CommandBuffers(const class CommandPool& commandPool, uint32_t size):
		device(commandPool.GetDevice()), commandPool(commandPool)
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool.Get();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = size;

		commandBuffers.resize(size);

		VK_CHECK(vkAllocateCommandBuffers(device.Get(), &allocInfo, commandBuffers.data()),
		         "Allocate command buffers");
	}

	CommandBuffers::~CommandBuffers()
	{
		if (!commandBuffers.empty())
		{
			vkFreeCommandBuffers(
				device.Get(), commandPool.Get(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
			commandBuffers.clear();
		}
	}

	VkCommandBuffer CommandBuffers::Begin(size_t i)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		VK_CHECK(vkBeginCommandBuffer(commandBuffers[i], &beginInfo), "Begin recording command buffer");

		return commandBuffers[i];
	}

	void CommandBuffers::End(size_t i)
	{
		VK_CHECK(vkEndCommandBuffer(commandBuffers[i]), "Record command buffer");
	}
}
