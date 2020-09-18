#pragma once

#include "Vulkan.h"

namespace Vulkan
{
	class Semaphore final
	{
	public:
		Semaphore(const class Device& device);
		~Semaphore();

		VkSemaphore GetRenderFinished() const { return RenderFinishedSemaphore; }

		VkSemaphore GetImageAvailable() const { return RenderFinishedSemaphore; }
		
	private:
		const Device& device;
		VkSemaphore RenderFinishedSemaphore;
		VkSemaphore ImageAvailableSemaphore;
	};
}
