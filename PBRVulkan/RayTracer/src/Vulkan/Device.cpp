#include "Device.h"

#include <set>

#include "Surface.h"

namespace Vulkan
{
	const std::vector<const char*> Device::RequiredExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_SHADER_CLOCK_EXTENSION_NAME,
		VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
		VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
		VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
		VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
		VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME
	};

	Device::Device(VkPhysicalDevice physicalDevice, const Surface& surface) :
		surface(surface), physicalDevice(physicalDevice)
	{
		if (!CheckDeviceExtensionSupport(physicalDevice))
		{
			throw std::runtime_error("The device is not suitable!");
		}

		auto indices = FindQueueFamilies(physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		float queuePriority = 1.0f;

		for (auto index : indices)
		{
			uint32_t queueFamilyIndex = index.family.value();

			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.fillModeNonSolid = VK_TRUE;
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.shaderInt64 = VK_TRUE;

		VkPhysicalDeviceShaderClockFeaturesKHR shaderClockFeatures = {};
		shaderClockFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR;
		shaderClockFeatures.pNext = nullptr;
		shaderClockFeatures.shaderSubgroupClock = true;

		VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures = {};
		bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
		bufferDeviceAddressFeatures.pNext = &shaderClockFeatures;
		bufferDeviceAddressFeatures.bufferDeviceAddress = true;

		VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures = {};
		indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
		indexingFeatures.pNext = &bufferDeviceAddressFeatures;
		indexingFeatures.runtimeDescriptorArray = true;
		indexingFeatures.shaderSampledImageArrayNonUniformIndexing = true;

		VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures = {};
		accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
		accelerationStructureFeatures.pNext = &indexingFeatures;
		accelerationStructureFeatures.accelerationStructure = true;

		VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingFeatures = {};
		rayTracingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
		rayTracingFeatures.pNext = &accelerationStructureFeatures;
		rayTracingFeatures.rayTracingPipeline = true;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext = &rayTracingFeatures;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledLayerCount = 0;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(RequiredExtensions.size());
		createInfo.ppEnabledExtensionNames = RequiredExtensions.data();

		VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device), "Create Vulkan logical device");

		for (auto index : indices)
		{
			uint32_t family = index.family.value();
			VkBool32 presentSupport = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, family, surface.Get(), &presentSupport);
			if (presentSupport)
			{
				vkGetDeviceQueue(device, family, 0, &PresentQueue);
				PresentFamilyIndex = family;
				break;
			}
		}

		for (auto index : indices)
		{
			uint32_t family = index.family.value();
			vkGetDeviceQueue(device, family, 0, &index.queue);
		}
	}

	std::vector<QueueFamily> Device::FindQueueFamilies(VkPhysicalDevice device)
	{
		std::vector<QueueFamily> indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				QueueFamily index{ GraphicsQueue };
				index.family = i;
				GraphicsFamilyIndex = i;
				indices.push_back(index);
			}
			else if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				QueueFamily index{ ComputeQueue };
				index.family = i;
				ComputeFamilyIndex = i;
				indices.push_back(index);
			}
			else if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				QueueFamily index{ TransferQueue };
				index.family = i;
				TransferFamilyIndex = i;
				indices.push_back(index);
			}

			i++;
		}

		return indices;
	}

	bool Device::CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(RequiredExtensions.begin(), RequiredExtensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	Device::~Device()
	{
		if (device != nullptr)
		{
			vkDestroyDevice(device, nullptr);
			device = nullptr;
		}
	}

	void Device::WaitIdle() const
	{
		VK_CHECK(vkDeviceWaitIdle(device), "Wait for a device to become idle");
	}
}
