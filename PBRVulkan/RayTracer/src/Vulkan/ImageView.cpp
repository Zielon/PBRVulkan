#include "ImageView.h"
#include "Device.h"

namespace Vulkan
{
	ImageView::ImageView(const Device& device, VkImage image, VkFormat format): ImageView(
		device, image, format, VK_IMAGE_ASPECT_COLOR_BIT) { }

	ImageView::ImageView(const Device& device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
		: device(device), image(image), format(format)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = aspectFlags;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VK_CHECK(vkCreateImageView(device.Get(), &createInfo, nullptr, &imageView), "Create image view");
	}

	ImageView::~ImageView()
	{
		if (imageView != nullptr)
		{
			vkDestroyImageView(device.Get(), imageView, nullptr);
			imageView = nullptr;
		}
	}
}
