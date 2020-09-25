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

	private:
		VkImage image{};
		VkExtent2D extent;
		VkFormat format;
		const Device& device;
		std::unique_ptr<Memory> memory;
	};
}
