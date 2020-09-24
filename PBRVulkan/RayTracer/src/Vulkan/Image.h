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

		void Copy(const class CommandBuffers& commandBuffers, const class Buffer& buffer);

		void TransitionImageLayout(
			const class CommandBuffers& commandBuffers,
			VkImageLayout oldLayout,
			VkImageLayout newLayout);

		[[nodiscard]] VkImage Get() const
		{
			return image;
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
