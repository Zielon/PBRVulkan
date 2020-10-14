#include "SwapChain.h"
#include "Device.h"
#include "Surface.h"
#include "Window.h"
#include "Instance.h"
#include "ImageView.h"

#include <algorithm>

namespace Vulkan
{
	SwapChain::SwapChain(const Device& device): device(device), surface(device.GetSurface())
	{
		const auto swapChainSupport = QuerySwapChainSupport(device.GetPhysical());

		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.PresentModes);
		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.Formats);
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.Capabilities);
		uint32_t imageCount = ChooseImageCount(swapChainSupport.Capabilities);

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface.Get();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = nullptr;

		if (device.GraphicsFamilyIndex != device.PresentFamilyIndex)
		{
			uint32_t queueFamilyIndices[] = { device.GraphicsFamilyIndex, device.PresentFamilyIndex };

			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		VK_CHECK(vkCreateSwapchainKHR(device.Get(), &createInfo, nullptr, &swapChain), "Create swap chain");

		vkGetSwapchainImagesKHR(device.Get(), swapChain, &imageCount, nullptr);
		images.resize(imageCount);
		vkGetSwapchainImagesKHR(device.Get(), swapChain, &imageCount, images.data());

		MinImageCount = swapChainSupport.Capabilities.minImageCount;
		PresentMode = presentMode;
		Format = surfaceFormat.format;
		Extent = extent;

		CreateImageViews();
	}

	SwapChain::~SwapChain()
	{
		imageViews.clear();

		if (swapChain != nullptr)
		{
			vkDestroySwapchainKHR(device.Get(), swapChain, nullptr);
			swapChain = nullptr;
		}
	}

	SwapChainSupportDetails SwapChain::QuerySwapChainSupport(VkPhysicalDevice device) const
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface.Get(), &details.Capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.Get(), &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.Formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.Get(), &formatCount, details.Formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.Get(), &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.PresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.Get(), &presentModeCount,
			                                          details.PresentModes.data());
		}

		return details;
	}

	VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}

		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace ==
				VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}

		auto actualExtent = surface.GetInstance().GetWindow().GetFramebufferSize();
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
		                                capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
		                                 capabilities.maxImageExtent.height);
		return actualExtent;
	}

	uint32_t SwapChain::ChooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		uint32_t imageCount = capabilities.minImageCount;

		if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
		{
			imageCount = capabilities.maxImageCount;
		}

		return imageCount;
	}

	void SwapChain::CreateImageViews()
	{
		for (auto* const image : images)
		{
			imageViews.
				push_back(std::make_unique<ImageView>(device, image, Format));
		}
	}
}
