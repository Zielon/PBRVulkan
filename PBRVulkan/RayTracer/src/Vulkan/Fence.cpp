#include "Fence.h"

#include "Device.h"

namespace Vulkan
{
	Fence::Fence(const Device& device) : device(device)
	{
		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VK_CHECK(vkCreateFence(device.Get(), &fenceInfo, nullptr, &fence), "Create fence");
	}

	Fence::~Fence()
	{
		if (fence != nullptr)
		{
			vkDestroyFence(device.Get(), fence, nullptr);
			fence = nullptr;
		}
	}

	void Fence::Reset() const
	{
		VK_CHECK(vkResetFences(device.Get(), 1, &fence), "Reset fence");
	}

	void Fence::Wait(uint64_t timeout) const
	{
		VK_CHECK(vkWaitForFences(device.Get(), 1, &fence, VK_TRUE, timeout), "Wait for fence");
	}
}
