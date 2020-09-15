#pragma once

#include "Vulkan.h"

#include <vector>

namespace Vulkan
{
	class Window;
	
	class Instance final
	{
	public:
		NON_COPIABLE(Instance)

		Instance(const Window& window, const std::vector<const char*>& validationLayers);
		~Instance();

		VkInstance Get() const { return instance; };
		const Window& GetWindow() const { return window; }
		const std::vector<VkPhysicalDevice>& GetDevices() const { return devices; }

	private:
		std::vector<const char*> GetRequiredInstanceExtensions() const;
		bool CheckValidationLayerSupport(const std::vector<const char*>&) const;
		void CheckPhysicalDevice();

		std::vector<VkPhysicalDevice> devices;
		VkInstance instance;
		const Window& window;
	};
}
