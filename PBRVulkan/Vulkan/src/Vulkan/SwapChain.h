#pragma once

#include <vector>

#include "Vulkan.h"

namespace Vulkan
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	};

	class SwapChain final
	{
	public:
		NON_COPIABLE(SwapChain)

		SwapChain(const class Device& device);
		~SwapChain();

		VkSwapchainKHR Get() const { return swapChain; };
		const std::vector<VkImage>& GetSwapChainImages() const { return swapChainImages; }

	private:
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) const;
		static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
		static uint32_t ChooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities);

		std::vector<VkImage> swapChainImages;
		VkSwapchainKHR swapChain;
		const class Device& device;
		const class Surface& surface;
	};
}
