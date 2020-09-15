#pragma once

#include "Vulkan.h"

#include <optional>
#include <vector>

namespace Vulkan
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		VkQueue& queue;

		QueueFamilyIndices(VkQueue& queue): queue(queue){}
		
		bool isComplete()
		{
			return graphicsFamily.has_value();
		}
	};

	class Surface;

	class Device final
	{
	public:
		NON_COPIABLE(Device);

		Device(VkPhysicalDevice physicalDevice, const Surface& surface);
		~Device();

		VkPhysicalDevice Get() const { return physicalDevice; };

	private:
		std::vector<QueueFamilyIndices> FindQueueFamilies(VkPhysicalDevice device);

		const VkPhysicalDevice physicalDevice;
		const Surface& surface;
		VkDevice device;

		VkQueue graphicsQueue{};
		VkQueue computeQueue{};
		VkQueue presentQueue{};
		VkQueue transferQueue{};
	};
}
