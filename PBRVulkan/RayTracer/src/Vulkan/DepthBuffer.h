#pragma once

#include <memory>

#include "ImageView.h"
#include "Image.h"
#include "Vulkan.h"

namespace Vulkan
{
	class DepthBuffer final
	{
	public:
		NON_COPIABLE(DepthBuffer)

		DepthBuffer(const class CommandPool& commandPool, VkExtent2D extent);
		~DepthBuffer() = default;

		[[nodiscard]] const class Image& GetImage() const
		{
			return *image;
		}

		[[nodiscard]] const class ImageView& GetImageView() const
		{
			return *imageView;
		}

	private:
		const class Device& device;
		std::unique_ptr<class Image> image;
		std::unique_ptr<class ImageView> imageView;
	};
}
