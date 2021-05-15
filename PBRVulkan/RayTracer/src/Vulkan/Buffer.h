#pragma once

#include "Vulkan.h"

#include <memory>

namespace Vulkan
{
	class Buffer final
	{
	public:
		NON_COPIABLE(Buffer)

		Buffer(const class Device& device, size_t size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags properties);
		
		Buffer(const class Device& device, size_t size, VkBufferUsageFlagBits usage,
		       VkMemoryAllocateFlags allocateFLags, VkMemoryPropertyFlags properties);
		
		~Buffer();

		void Copy(const class CommandPool& commandPool, const Buffer& src);
		void Fill(const void* data) const;
		void Unmap() const;

		[[nodiscard]] void* Map(size_t offset, size_t size) const;

		[[nodiscard]] VkDeviceAddress GetDeviceAddress() const;

		[[nodiscard]] const VkBuffer& Get() const
		{
			return buffer;
		}

		[[nodiscard]] const Device& GetDevice() const
		{
			return device;
		}

		[[nodiscard]] const class Memory& GetMemory() const
		{
			return *memory;
		}

	private:
		VkDeviceSize size;
		VkBuffer buffer{};
		const Device& device;
		std::unique_ptr<Memory> memory;

		[[nodiscard]] VkMemoryRequirements GetMemoryRequirements() const;
	};
}
