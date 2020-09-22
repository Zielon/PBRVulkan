#include <functional>

#include "Vulkan.h"
#include "CommandBuffers.h"
#include "Device.h"

namespace Vulkan
{
	class Command final
	{
	public:
		static void Submit(const CommandBuffers& commandBuffers, const std::function<void(VkCommandBuffer)>& func)
		{
			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = commandBuffers.GetPool();
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(commandBuffers.GetDevice().Get(), &allocInfo, &commandBuffer);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(commandBuffer, &beginInfo);

			func(commandBuffer);
			
			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			const auto graphicsQueue = commandBuffers.GetDevice().GraphicsQueue;

			vkQueueSubmit(graphicsQueue, 1, &submitInfo, nullptr);
			vkQueueWaitIdle(graphicsQueue);
		}
	};
}
