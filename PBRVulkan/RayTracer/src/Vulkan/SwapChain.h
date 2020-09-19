#pragma once

#include <vector>
#include <memory>

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

		[[nodiscard]] VkSwapchainKHR Get() const { return swapChain; };
		[[nodiscard]] const std::vector<VkImage>& GetImage() const { return images; }
		[[nodiscard]] const std::vector<std::unique_ptr<class ImageView>>& GetImageViews() const { return imageViews; }

	private:
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) const;
		static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
		static uint32_t ChooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities);
		void CreateImageViews();

		std::vector<VkImage> images;
		std::vector<std::unique_ptr<class ImageView>> imageViews;
		VkSwapchainKHR swapChain;
		const class Device& device;
		const class Surface& surface;

	public:
		VkPresentModeKHR PresentMode{};
		VkFormat Format{};
		VkExtent2D Extent{};
		uint32_t MinImageCount{};
	};
}
