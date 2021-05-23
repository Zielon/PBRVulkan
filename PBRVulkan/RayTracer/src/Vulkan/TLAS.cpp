#include "TLAS.h"

#include "BLAS.h"
#include "Device.h"
#include "Buffer.h"
#include "Memory.h"
#include "Extensions.h"

namespace Vulkan
{
	TLAS::TLAS(TLAS&& other) noexcept
		: AccelerationStructure(std::move(other)), instancesCount(other.instancesCount) { }

	TLAS::TLAS(const class Device& device, VkDeviceAddress instanceAddress, uint32_t instancesCount):
		AccelerationStructure(device), instancesCount(instancesCount)
	{
		instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
		instances.arrayOfPointers = VK_FALSE;
		instances.data.deviceAddress = instanceAddress;

		geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
		geometry.geometry.instances = instances;

		buildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		buildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		buildGeometryInfo.geometryCount = 1;
		buildGeometryInfo.pGeometries = &geometry;
		buildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		buildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
		buildGeometryInfo.srcAccelerationStructure = nullptr;

		buildSizesInfo = GetMemorySizes(&instancesCount);
	}

	void TLAS::Generate(
		VkCommandBuffer commandBuffer,
		class Buffer& topScratchBuffer,
		VkDeviceSize scratchOffset,
		class Buffer& topBuffer,
		VkDeviceSize topOffset)
	{
		VkAccelerationStructureCreateInfoKHR createInfo = {};

		createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		createInfo.pNext = nullptr;
		createInfo.type = buildGeometryInfo.type;
		createInfo.size = buildSizesInfo.accelerationStructureSize;
		createInfo.buffer = topBuffer.Get();
		createInfo.offset = topOffset;

		VK_CHECK(
			extensions->vkCreateAccelerationStructureKHR(device.Get(), &createInfo, nullptr, &accelerationStructure),
			"Create acceleration structure");

		// Build the actual bottom-level acceleration structure
		VkAccelerationStructureBuildRangeInfoKHR buildOffsetInfo = {};
		buildOffsetInfo.primitiveCount = instancesCount;

		const VkAccelerationStructureBuildRangeInfoKHR* pBuildOffsetInfo = &buildOffsetInfo;

		buildGeometryInfo.dstAccelerationStructure = accelerationStructure;
		buildGeometryInfo.scratchData.deviceAddress = topScratchBuffer.GetDeviceAddress() + scratchOffset;

		extensions->vkCmdBuildAccelerationStructuresKHR(commandBuffer, 1, &buildGeometryInfo, &pBuildOffsetInfo);
	}

	VkAccelerationStructureInstanceKHR TLAS::CreateInstance(
		const BLAS& blas,
		const glm::mat4& transform,
		uint32_t instanceId)
	{
		const auto& device = blas.GetDevice();
		const auto& extenstion = blas.GetExtensions();

		VkAccelerationStructureDeviceAddressInfoKHR addressInfo = {};

		addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
		addressInfo.accelerationStructure = blas.Get();

		const VkDeviceAddress address = extenstion.vkGetAccelerationStructureDeviceAddressKHR(
			device.Get(), &addressInfo);

		VkAccelerationStructureInstanceKHR geometryInstance = {};

		std::memcpy(&geometryInstance.transform, &transform, sizeof(glm::mat4));
		geometryInstance.instanceCustomIndex = instanceId;
		geometryInstance.mask = 0xFF;
		geometryInstance.instanceShaderBindingTableRecordOffset = 0;
		geometryInstance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
		geometryInstance.accelerationStructureReference = address;

		return geometryInstance;
	}
}
