#include "TextureImage.h"

#include "../Vulkan/Buffer.h"
#include "../Vulkan/CommandPool.h"

#include "../Assets/Texture.h"

namespace Tracer
{
	TextureImage::TextureImage(const Vulkan::Device& device,
	                           const Vulkan::CommandPool& commandPool,
	                           const Assets::Texture& texture,
	                           VkFormat format,
	                           VkImageTiling tiling,
	                           VkImageType imageType)
	{
		const std::unique_ptr<Vulkan::Buffer> stagingBuffer(new Vulkan::Buffer(
			device, texture.GetImageSize(),
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

		stagingBuffer->Fill(texture.GetPixels());

		const auto extent = VkExtent2D{
			static_cast<uint32_t>(texture.GetWidth()), static_cast<uint32_t>(texture.GetHeight())
		};

		image.reset(new Vulkan::Image(
			device, extent, format, tiling, imageType,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		image->TransitionImageLayout(commandPool, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		image->Copy(commandPool, *stagingBuffer);
		image->TransitionImageLayout(commandPool, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		                             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		sampler.reset(new Vulkan::TextureSampler(device));

		imageView.reset(new Vulkan::ImageView(device, image->Get(), image->GetFormat()));
	}
}
