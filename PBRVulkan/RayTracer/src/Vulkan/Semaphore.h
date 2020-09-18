#pragma once

#include "Vulkan.h"

namespace Vulkan
{
	class Semaphore final
	{
	public:
		NON_COPIABLE(Semaphore)

		Semaphore(const class Device& device);
		~Semaphore();

		[[nodiscard]] VkSemaphore GetRenderFinished() const { return RenderFinishedSemaphore; }
		[[nodiscard]] VkSemaphore GetImageAvailable() const { return RenderFinishedSemaphore; }

	private:
		const Device& device;
		VkSemaphore RenderFinishedSemaphore;
		VkSemaphore ImageAvailableSemaphore;
	};
}
