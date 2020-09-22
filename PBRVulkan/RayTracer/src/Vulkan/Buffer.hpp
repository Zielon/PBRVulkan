#pragma once

#include "Vulkan.h"
#include "Device.h"
#include "Memory.h"
#include "CommandBuffers.h"
#include "Command.cpp"

#include <memory>

namespace Vulkan
{
	class Device;

	template <class T>
	class Buffer final
	{
	public:
		NON_COPIABLE(Buffer)

		Buffer(const Device& device, size_t size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags properties);
		~Buffer();

		void Copy(const CommandBuffers& commandBuffers, const Buffer& src);
		void Fill(const void* data) const;

		[[nodiscard]] const VkBuffer& Get() const
		{
			return buffer;
		}

		[[nodiscard]] const Device& GetDevice() const
		{
			return device;
		}

		[[nodiscard]] const Memory& GetMemory() const
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

	template <class T>
	Buffer<T>::Buffer(
		const Device& device,
		size_t size,
		VkBufferUsageFlagBits usage,
		VkMemoryPropertyFlags properties): device(device), size(size)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferInfo.size = size;
		bufferInfo.usage = usage;

		VK_CHECK(vkCreateBuffer(device.Get(), &bufferInfo, nullptr, &buffer), "Create buffer");

		memory.reset(new Memory(device, GetMemoryRequirements(), properties));

		VK_CHECK(vkBindBufferMemory(device.Get(), buffer, memory->Get(), 0), "Bind buffer memory");
	}

	template <class T>
	Buffer<T>::~Buffer()
	{
		if (buffer != nullptr)
		{
			vkDestroyBuffer(device.Get(), buffer, nullptr);
			buffer = nullptr;
		}
	}

	template <class T>
	void Buffer<T>::Copy(const CommandBuffers& commandBuffers, const Buffer& src)
	{
		Command::Submit(commandBuffers, [&](VkCommandBuffer cmd)-> void
		{
			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0;
			copyRegion.size = size;

			vkCmdCopyBuffer(cmd, src.Get(), buffer, 1, &copyRegion);
		});
	}

	template <class T>
	void Buffer<T>::Fill(const void* data) const
	{
		memory->Fill(data, size);
	}

	template <class T>
	VkMemoryRequirements Buffer<T>::GetMemoryRequirements() const
	{
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device.Get(), buffer, &memRequirements);
		return memRequirements;
	}
}
