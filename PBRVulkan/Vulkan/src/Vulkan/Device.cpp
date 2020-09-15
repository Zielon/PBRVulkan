#include "Device.h"

namespace Vulkan
{
	Device::Device(VkPhysicalDevice physicalDevice, const Surface& surface) :
		physicalDevice(physicalDevice), surface(surface)
	{
		auto indices = FindQueueFamilies(physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		float queuePriority = 1.0f;

		for (auto index : indices)
		{
			uint32_t queueFamilyIndex = index.graphicsFamily.value();

			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.fillModeNonSolid = true;
		deviceFeatures.samplerAnisotropy = true;

		VkPhysicalDeviceDescriptorIndexingFeaturesEXT indexingFeatures = {};
		indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
		indexingFeatures.runtimeDescriptorArray = true;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext = &indexingFeatures;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;

		VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device), "Create Vulkan logical device");

		for (auto index : indices)
		{
			uint32_t queueFamilyIndex = index.graphicsFamily.value();
			vkGetDeviceQueue(device, queueFamilyIndex, 0, &index.queue);
		}
	}

	std::vector<QueueFamilyIndices> Device::FindQueueFamilies(VkPhysicalDevice device)
	{
		std::vector<QueueFamilyIndices> indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				QueueFamilyIndices index{graphicsQueue};
				index.graphicsFamily = i;
				indices.push_back(index);
			}

			if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT &&
				!(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
			{
				QueueFamilyIndices index{computeQueue};
				index.graphicsFamily = i;
				indices.push_back(index);
			}

			if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT &&
				!(queueFamily.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)))
			{
				QueueFamilyIndices index{transferQueue};
				index.graphicsFamily = i;
				indices.push_back(index);
			}

			i++;
		}

		return indices;
	}

	Device::~Device()
	{
		if (device != nullptr)
		{
			vkDestroyDevice(device, nullptr);
			device = nullptr;
		}
	}
}
