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

		accelerationProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
		pipelineRTProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
		pipelineRTProperties.pNext = &accelerationProperties;

		VkPhysicalDeviceProperties2 props = {};
		props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		props.pNext = &pipelineRTProperties;
		vkGetPhysicalDeviceProperties2(device.GetPhysical(), &props);
	}

	uint64_t AccelerationStructure::RoundUp(uint64_t numToRound, uint64_t multiple)
	{
		return ((numToRound + multiple - 1) / multiple) * multiple;
	}

	AccelerationStructure::AccelerationStructure(AccelerationStructure&& other) noexcept :
		buildGeometryInfo(other.buildGeometryInfo),
		buildSizesInfo(other.buildSizesInfo),
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

		sizeInfo.accelerationStructureSize = RoundUp(sizeInfo.accelerationStructureSize, 256);
		sizeInfo.buildScratchSize = RoundUp(sizeInfo.buildScratchSize,
		                                    accelerationProperties.minAccelerationStructureScratchOffsetAlignment);

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
