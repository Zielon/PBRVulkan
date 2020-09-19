#include "Semaphore.h"

#include "Device.h"

namespace Vulkan
{
	Semaphore::Semaphore(const class Device& device): device(device)
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VK_CHECK(vkCreateSemaphore(device.Get(), &semaphoreInfo, nullptr, &semaphore),
		         "Create image Available Semaphore");
	}

	Semaphore::~Semaphore()
	{
		if (semaphore != nullptr)
		{
			vkDestroySemaphore(device.Get(), semaphore, nullptr);
			semaphore = nullptr;
		}
	}
}
