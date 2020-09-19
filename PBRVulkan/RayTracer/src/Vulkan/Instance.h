#pragma once

#include "Vulkan.h"

#include <vector>

namespace Vulkan
{
	class Window;

	/*
	 * The instance is the connection between your application and the Vulkan library
	 * and creating it involves specifying some details about your application to the driver.
	 */
	class Instance final
	{
	public:
		NON_COPIABLE(Instance)

		Instance(const Window& window, const std::vector<const char*>& validationLayers);
		~Instance();

		[[nodiscard]] VkInstance Get() const
		{
			return instance;
		}

		[[nodiscard]] const Window& GetWindow() const
		{
			return window;
		}

		[[nodiscard]] const std::vector<VkPhysicalDevice>& GetDevices() const
		{
			return devices;
		}

	private:
		[[nodiscard]] std::vector<const char*> GetRequiredInstanceExtensions() const;
		bool CheckValidationLayerSupport(const std::vector<const char*>&) const;
		void CheckPhysicalDevice();

		std::vector<VkPhysicalDevice> devices;
		VkInstance instance{};
		const Window& window;
	};
}
