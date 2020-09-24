#include "TextureImage.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "../Vulkan/Buffer.h"
#include "../Vulkan/Image.h"
#include "../Vulkan/CommandBuffers.h"

namespace Assets
{
	TextureImage::TextureImage(const Vulkan::Device& device,
	                           const Vulkan::CommandBuffers& commandBuffers,
	                           const std::string& path)
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texHeight * texWidth * 4;

		if (!pixels)
		{
			throw std::runtime_error("Failed to load texture image!");
		}

		std::unique_ptr<Vulkan::Buffer> stagingBuffer(new Vulkan::Buffer(
			device, imageSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

		stagingBuffer->Fill(pixels);

		stbi_image_free(pixels);

		const auto extent = VkExtent2D{ static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight) };

		image.reset(new Vulkan::Image(
			device,
			extent,
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		image->TransitionImageLayout(commandBuffers, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		image->Copy(commandBuffers, *stagingBuffer);
		image->TransitionImageLayout(commandBuffers, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		                             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
}
