#pragma once

#include "Vulkan.h"

namespace Vulkan
{
	class Device;

	/*
	 * Class which handles memory allocation for buffers.
	 */
	class Memory final
	{
	public:
		NON_COPIABLE(Memory)

		Memory(const Device& device, VkMemoryRequirements requirements, VkMemoryPropertyFlags properties);
		~Memory();

		void Fill(const void* data, size_t size) const;
		void Unmap() const;
		
		[[nodiscard]] void* Map(size_t offset, size_t size) const;

		[[nodiscard]] const VkDeviceMemory& Get() const
		{
			return memory;
		}

		[[nodiscard]] const class Device& GetDevice() const
		{
			return device;
		}

	private:
		const Device& device;
		VkDeviceMemory memory;
		[[nodiscard]] uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
	};
}
