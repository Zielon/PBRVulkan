#pragma once

#include "Vulkan.h"

#include <optional>
#include <vector>

namespace Vulkan
{
	struct QueueFamily
	{
		std::optional<uint32_t> family;
		VkQueue& queue;

		explicit QueueFamily(VkQueue& queue): queue(queue) { }
	};

	class Surface;

	/**
	 * Vulkan's logical device. Additionally, the class contains different queues.
	 * Every operation in Vulkan requires commands to be submitted to a queue.
	 * There are different types of queues that originate from different
	 * queue families and each family of queues allows only a subset of commands.
	 */
	class Device final
	{
	public:
		NON_COPIABLE(Device);

		Device(VkPhysicalDevice physicalDevice, const Surface& surface);
		~Device();

		[[nodiscard]] VkDevice Get() const { return device; }
		[[nodiscard]] VkPhysicalDevice GetPhysical() const { return physicalDevice; }
		[[nodiscard]] const class Surface& GetSurface() const { return surface; }

	private:
		std::vector<QueueFamily> FindQueueFamilies(VkPhysicalDevice device);
		static bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice);

		static const std::vector<const char*> RequiredExtensions;
		const Surface& surface;
		VkPhysicalDevice physicalDevice;
		VkDevice device{};

	public:
		uint32_t GraphicsFamilyIndex{};
		uint32_t ComputeFamilyIndex{};
		uint32_t PresentFamilyIndex{};
		uint32_t TransferFamilyIndex{};
		VkQueue GraphicsQueue{};
		VkQueue ComputeQueue{};
		VkQueue PresentQueue{};
		VkQueue TransferQueue{};
	};
}
