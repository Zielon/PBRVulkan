#pragma once

#include "Vulkan.h"

#include <memory>

namespace Vulkan
{
	class Image final
	{
	public:
		NON_COPIABLE(Image)

		Image(const class Device& device,
		      VkExtent2D extent,
		      VkFormat format,
		      VkImageTiling tiling,
		      VkImageType imageType,
		      VkImageUsageFlags usage,
		      VkMemoryPropertyFlags properties);
		~Image();

		void Copy(const class CommandPool& commandPool, const class Buffer& buffer);

		void TransitionImageLayout(
			const class CommandPool& commandPool,
			VkImageLayout oldLayout,
			VkImageLayout newLayout);

		[[nodiscard]] VkImage Get() const
		{
			return image;
		}

		[[nodiscard]] VkFormat GetFormat() const
		{
			return format;
		}

		[[nodiscard]] const class Memory& GetMemory() const
		{
			return *memory;
		}

		[[nodiscard]] const class Device& GetDevice() const
		{
			return device;
		}

		static VkClearColorValue GetColor(float r, float g, float b);
		
		static VkImageSubresourceRange GetSubresourceRange();

		static VkImageCopy GetImageCopy(uint32_t width, uint32_t height);

		static void MemoryBarrier(
			VkCommandBuffer commandBuffer,
			VkImage image,
			VkImageSubresourceRange subresourceRange,
			VkAccessFlags srcAccessMask,
			VkAccessFlags dstAccessMask,
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			VkPipelineStageFlagBits srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VkPipelineStageFlagBits dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
		);

	private:
		VkImage image{};
		VkExtent2D extent;
		VkFormat format;
		VkImageType imageType;
		const Device& device;
		std::unique_ptr<Memory> memory;
	};
}
