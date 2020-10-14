#include "Memory.h"
#include "Device.h"

namespace Vulkan
{
	Memory::Memory(const Device& device,
	               VkMemoryRequirements requirements,
	               VkMemoryPropertyFlags properties): device(device)
	{
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = requirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(requirements.memoryTypeBits, properties);

		VK_CHECK(vkAllocateMemory(device.Get(), &allocInfo, nullptr, &memory),
		         "Allocate memory");
	}

	Memory::~Memory()
	{
		if (memory != nullptr)
		{
			vkFreeMemory(device.Get(), memory, nullptr);
			memory = nullptr;
		}
	}

	void Memory::Fill(const void* data, size_t size) const
	{
		void* location;
		vkMapMemory(device.Get(), memory, 0, size, 0, &location);
		memcpy(location, data, size);
		vkUnmapMemory(device.Get(), memory);
	}

	void* Memory::Map(size_t offset, size_t size) const
	{
		void* data;
		VK_CHECK(vkMapMemory(device.Get(), memory, offset, size, 0, &data), "Map memory");
		return data;
	}

	void Memory::Unmap() const
	{
		vkUnmapMemory(device.Get(), memory);
	}

	uint32_t Memory::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
	{
		VkPhysicalDeviceMemoryProperties memProperties;

		vkGetPhysicalDeviceMemoryProperties(device.GetPhysical(), &memProperties);

		for (uint32_t i = 0; i != memProperties.memoryTypeCount; ++i)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memory type!");
	}
}
