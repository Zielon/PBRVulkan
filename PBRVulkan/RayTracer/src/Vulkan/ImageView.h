#pragma once

#include "Vulkan.h"

namespace Vulkan
{
	class ImageView final
	{
	public:
		NON_COPIABLE(ImageView)

		ImageView(const class Device& device, VkImage image, VkFormat format);
		~ImageView();

		VkImageView Get() const { return imageView; }

		const class Device& GetDevice() const { return device; }

	private:
		VkImageView imageView;
		const class Device& device;
		const VkImage image;
		const VkFormat format;
	};
}
