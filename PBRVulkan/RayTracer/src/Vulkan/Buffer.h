#pragma once

#include "Vulkan.h"

#include <memory>

namespace Vulkan
{
	class Device;

	class Buffer final
	{
	public:
		NON_COPIABLE(Buffer)

		Buffer(const Device& device, size_t size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags properties);
		~Buffer();

		void Copy(const class CommandBuffers& commandBuffers, const Buffer& src);
		void Fill(const void* data) const;

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
		VkBuffer buffer{};
		const Device& device;
		std::unique_ptr<Memory> memory;
		VkDeviceSize size;

		[[nodiscard]] VkMemoryRequirements GetMemoryRequirements() const;
	};
}
