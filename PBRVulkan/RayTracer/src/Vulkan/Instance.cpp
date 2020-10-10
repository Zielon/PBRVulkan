#include "Instance.h"

namespace Vulkan
{
	Instance::Instance(const Window& window, const std::vector<const char*>& validationLayers): window(window)
	{
		// Application info
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "VulkanRayTracer";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_1;

		auto extensions = GetRequiredInstanceExtensions();

		if (!validationLayers.empty())
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		// Vulkan instnace
		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();


		if (CheckValidationLayerSupport(validationLayers))
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}

		VK_CHECK(vkCreateInstance(&createInfo, nullptr, &instance), "Vulkan MeshInstance creation");

		CheckPhysicalDevice();
	}

	Instance::~Instance()
	{
		if (instance != nullptr)
		{
			vkDestroyInstance(instance, nullptr);
			instance = nullptr;
		}
	}

	std::vector<const char*> Instance::GetRequiredInstanceExtensions() const
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
	}

	bool Instance::CheckValidationLayerSupport(const std::vector<const char*>& validationLayers) const
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layer : validationLayers)
		{
			auto result = std::find_if(availableLayers.begin(), availableLayers.end(),
			                           [layer](const VkLayerProperties& layerProperties)
			                           {
				                           return strcmp(layer, layerProperties.layerName) == 0;
			                           });

			// Not found
			if (result == availableLayers.end())
			{
				return false;
			}
		}

		return true;
	}

	void Instance::CheckPhysicalDevice()
	{
		VkPhysicalDevice physicalDevice = nullptr;
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			throw std::runtime_error("Failed to find GPUs with Vulkan support!");
		}

		devices.resize(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	}
}
