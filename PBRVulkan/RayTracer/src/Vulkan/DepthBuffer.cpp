#include "DepthBuffer.h"

#include "Device.h"
#include "CommandPool.h"

namespace Vulkan
{
	DepthBuffer::DepthBuffer(const CommandPool& commandPool, VkExtent2D extent): device(commandPool.GetDevice())
	{
		image.reset(new Image(
			device, extent,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_TYPE_2D,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		imageView.reset(new ImageView(commandPool.GetDevice(), image->Get(), image->GetFormat(),
		                              VK_IMAGE_ASPECT_DEPTH_BIT));

		image->TransitionImageLayout(commandPool, VK_IMAGE_LAYOUT_UNDEFINED,
		                             VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}
}
