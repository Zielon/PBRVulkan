#include "Buffer.h"

#include "Device.h"
#include "Memory.h"
#include "CommandPool.h"
#include "Command.cpp"

namespace Vulkan
{
	Buffer::Buffer(const Device& device, size_t size, VkBufferUsageFlagBits usage,
	               VkMemoryPropertyFlags properties) : size(size), device(device)
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

	Buffer::~Buffer()
	{
		if (buffer != nullptr)
		{
			vkDestroyBuffer(device.Get(), buffer, nullptr);
			buffer = nullptr;
		}
	}

	void Buffer::Copy(const CommandPool& commandPool, const Buffer& src)
	{
		Command::Submit(commandPool, [&](VkCommandBuffer cmd)-> void
		{
			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0;
			copyRegion.size = size;

			vkCmdCopyBuffer(cmd, src.Get(), buffer, 1, &copyRegion);
		});
	}

	void Buffer::Fill(const void* data) const
	{
		memory->Fill(data, size);
	}

	void Buffer::Unmap() const
	{
		memory->Unmap();
	}

	void* Buffer::Map(size_t offset, size_t size) const
	{
		return memory->Map(offset, size);
	}

	VkMemoryRequirements Buffer::GetMemoryRequirements() const
	{
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device.Get(), buffer, &memRequirements);
		return memRequirements;
	}
}
