#include "Semaphore.h"

#include "Device.h"

namespace Vulkan
{
	Semaphore::Semaphore(const class Device& device): device(device)
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VK_CHECK(vkCreateSemaphore(device.Get(), &semaphoreInfo, nullptr, &ImageAvailableSemaphore),
		         "Create image Available Semaphore");
		VK_CHECK(vkCreateSemaphore(device.Get(), &semaphoreInfo, nullptr, &RenderFinishedSemaphore),
		         "Create render Finished Semaphore");
	}

	Semaphore::~Semaphore()
	{
		if (ImageAvailableSemaphore != nullptr && RenderFinishedSemaphore != nullptr)
		{
			vkDestroySemaphore(device.Get(), ImageAvailableSemaphore, nullptr);
			vkDestroySemaphore(device.Get(), RenderFinishedSemaphore, nullptr);
			RenderFinishedSemaphore = nullptr;
			ImageAvailableSemaphore = nullptr;
		}
	}
}
