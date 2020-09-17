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
		QueueFamily(VkQueue& queue): queue(queue){}
	};

	class Surface;

	class Device final
	{
	public:
		NON_COPIABLE(Device);

		Device(VkPhysicalDevice physicalDevice, const Surface& surface);
		~Device();

		VkDevice Get() const { return device; }
		VkPhysicalDevice GetPhysical() const { return physicalDevice; }
		const class Surface& GetSurface() const { return surface; }

	private:
		std::vector<QueueFamily> FindQueueFamilies(VkPhysicalDevice device);
		static bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice);

		static const std::vector<const char*> RequiredExtensions;
		const VkPhysicalDevice physicalDevice;
		const Surface& surface;
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
