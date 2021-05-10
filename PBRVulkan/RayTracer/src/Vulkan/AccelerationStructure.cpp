#include "AccelerationStructure.h"

#include "Buffer.h"
#include "Device.h"
#include "Extensions.h"

#undef MemoryBarrier

namespace Vulkan
{
	AccelerationStructure::AccelerationStructure(const class Device& device) : device(device)
	{
		extensions.reset(new Extensions(device));
	}

	AccelerationStructure::AccelerationStructure(AccelerationStructure&& other) noexcept :
		device(other.device),
		extensions(std::move(other.extensions)),
		accelerationStructure(other.accelerationStructure)
	{
		other.accelerationStructure = nullptr;
	}

	AccelerationStructure::~AccelerationStructure()
	{
		if (accelerationStructure != nullptr)
		{
			extensions->vkDestroyAccelerationStructureKHR(device.Get(), accelerationStructure, nullptr);
			accelerationStructure = nullptr;
		}
	}

	void AccelerationStructure::Create(const Buffer& buffer, VkDeviceSize resultOffset)
	{
		VkAccelerationStructureCreateInfoKHR createInfo = {};

		createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		createInfo.pNext = nullptr;
		createInfo.type = buildGeometryInfo.type;
		createInfo.size = buildSizesInfo.accelerationStructureSize;
		createInfo.buffer = buffer.Get();
		createInfo.offset = resultOffset;

		VK_CHECK(
			extensions->vkCreateAccelerationStructureKHR(
				device.Get(), &createInfo, nullptr, &accelerationStructure),
			"Create acceleration structure");
	}

	VkAccelerationStructureBuildSizesInfoKHR AccelerationStructure::GetMemorySizes(const uint32_t* count) const
	{
		VkAccelerationStructureBuildSizesInfoKHR sizeInfo = {};

		sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

		extensions->vkGetAccelerationStructureBuildSizesKHR(
			device.Get(),
			VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
			&buildGeometryInfo, count, &sizeInfo);

		sizeInfo.accelerationStructureSize = 0;
		sizeInfo.buildScratchSize = 0;

		return sizeInfo;
	}

	void AccelerationStructure::MemoryBarrier(VkCommandBuffer commandBuffer)
	{
		auto flags = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

		VkMemoryBarrier memoryBarrier = {};

		memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		memoryBarrier.pNext = nullptr;
		memoryBarrier.srcAccessMask = flags;
		memoryBarrier.dstAccessMask = flags;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
			VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
			0, 1, &memoryBarrier, 0, nullptr, 0, nullptr);
	}
}
