#include "TLAS.h"

#include "BLAS.h"
#include "Device.h"
#include "Buffer.h"
#include "Memory.h"
#include "Extensions.h"

namespace Vulkan
{
	namespace TLASHelper
	{
		VkAccelerationStructureCreateInfoNV GetCreateInfo(const size_t instanceCount, const bool allowUpdate)
		{
			const auto flags = allowUpdate
				                   ? VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_NV
				                   : VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_NV;

			VkAccelerationStructureCreateInfoNV structureInfo = {};
			structureInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV;
			structureInfo.pNext = nullptr;
			structureInfo.info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
			structureInfo.info.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV;
			structureInfo.info.flags = flags;
			structureInfo.compactedSize = 0;
			structureInfo.info.instanceCount = static_cast<uint32_t>(instanceCount);
			structureInfo.info.geometryCount = 0; // Since this is a top-level AS, it does not contain any geometry
			structureInfo.info.pGeometries = nullptr;

			return structureInfo;
		}
	}

	TLAS::TLAS(
		const class Device& device,
		const std::vector<VkGeometryInstance>& geometryInstances,
		bool allowUpdate) :
		AccelerationStructure(device, TLASHelper::GetCreateInfo(geometryInstances.size(), allowUpdate)),
		geometryInstances(geometryInstances) {}

	TLAS::TLAS(TLAS&& other) noexcept :
		AccelerationStructure(std::move(other)),
		geometryInstances(std::move(other.geometryInstances)) {}

	void TLAS::Generate(
		VkCommandBuffer commandBuffer,
		class Buffer& topScratchBuffer,
		VkDeviceSize scratchOffset,
		class Buffer& topBuffer,
		VkDeviceSize topOffset,
		class Buffer& instanceBuffer,
		VkDeviceSize instanceOffset) const
	{
		// Copy the instance descriptors into the provider buffer.
		const auto instancesBufferSize = geometryInstances.size() * sizeof(VkGeometryInstance);
		void* data = instanceBuffer.Map(0, instancesBufferSize);
		std::memcpy(data, geometryInstances.data(), instancesBufferSize);

		// Bind the acceleration structure descriptor to the actual memory that will contain it
		VkBindAccelerationStructureMemoryInfoNV bindInfo = {};
		bindInfo.sType = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_NV;
		bindInfo.pNext = nullptr;
		bindInfo.accelerationStructure = accelerationStructure;
		bindInfo.memory = topBuffer.GetMemory().Get();
		bindInfo.memoryOffset = topOffset;
		bindInfo.deviceIndexCount = 0;
		bindInfo.pDeviceIndices = nullptr;

		VK_CHECK(extensions->vkBindAccelerationStructureMemoryNV(device.Get(), 1, &bindInfo),
		         "Bind acceleration structure");

		// Build the actual bottom-level acceleration structure
		const auto flags = allowUpdate
			                   ? VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_NV
			                   : VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_NV;

		VkAccelerationStructureInfoNV buildInfo = {};
		buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
		buildInfo.pNext = nullptr;
		buildInfo.flags = flags;
		buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV;
		buildInfo.instanceCount = static_cast<uint32_t>(geometryInstances.size());
		buildInfo.geometryCount = 0;
		buildInfo.pGeometries = nullptr;

		extensions->vkCmdBuildAccelerationStructureNV(
			commandBuffer, &buildInfo, instanceBuffer.Get(), instanceOffset, updateOnly, accelerationStructure,
			previousStructure, topScratchBuffer.Get(), scratchOffset);
	}

	VkGeometryInstance TLAS::CreateGeometryInstance(
		const BLAS& blas,
		const glm::mat4& transform,
		uint32_t instanceId)
	{
		const auto& device = blas.GetDevice();
		const auto& extenstion = blas.GetExtensions();

		uint64_t handle;
		VK_CHECK(extenstion.vkGetAccelerationStructureHandleNV(device.Get(), blas.Get(), sizeof(uint64_t), &handle),
		         "Get acceleration structure handle");

		VkGeometryInstance geometryInstance = {};
		std::memcpy(geometryInstance.transform, &transform, sizeof(glm::mat4));
		geometryInstance.instanceCustomIndex = instanceId;
		geometryInstance.mask = 0xFF;
		// The visibility mask is always set of 0xFF, but if some instances would need to be ignored in some cases, this flag should be passed by the application.
		geometryInstance.instanceOffset = 0;
		// Set the hit group index, that will be used to find the shader code to execute when hitting the geometry.
		geometryInstance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_CULL_DISABLE_BIT_NV;
		// Disable culling - more fine control could be provided by the application
		geometryInstance.accelerationStructureHandle = handle;

		return geometryInstance;
	}
}
