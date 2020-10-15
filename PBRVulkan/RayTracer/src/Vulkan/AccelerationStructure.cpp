#include "AccelerationStructure.h"

#include "Device.h"
#include "Extensions.h"

#undef MemoryBarrier

namespace Vulkan
{
	AccelerationStructure::AccelerationStructure(
		const class Device& device,
		const VkAccelerationStructureCreateInfoNV& createInfo) :
		allowUpdate(createInfo.info.flags & VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_NV), device(device)
	{
		extensions.reset(new Extensions(device));

		VK_CHECK(
			extensions->vkCreateAccelerationStructureNV(device.Get(), &createInfo, nullptr, &accelerationStructure),
			"Create acceleration structure");
	}

	AccelerationStructure::AccelerationStructure(AccelerationStructure&& other) noexcept :
		allowUpdate(other.allowUpdate),
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
			extensions->vkDestroyAccelerationStructureNV(device.Get(), accelerationStructure, nullptr);
			accelerationStructure = nullptr;
		}
	}

	ASMemoryRequirementsNV AccelerationStructure::GetMemoryRequirements() const
	{
		VkAccelerationStructureMemoryRequirementsInfoNV ASMemoryRequirementsInfoNV{};
		ASMemoryRequirementsInfoNV.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV;
		ASMemoryRequirementsInfoNV.pNext = nullptr;
		ASMemoryRequirementsInfoNV.accelerationStructure = accelerationStructure;

		// If the descriptor already contains the geometry info, so we can directly compute the estimated size and required scratch memory.
		VkMemoryRequirements2 memoryRequirements = {};
		ASMemoryRequirementsInfoNV.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_NV;
		extensions->vkGetAccelerationStructureMemoryRequirementsNV(
			device.Get(), &ASMemoryRequirementsInfoNV, &memoryRequirements);

		const auto resultRequirements = memoryRequirements.memoryRequirements;

		ASMemoryRequirementsInfoNV.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_BUILD_SCRATCH_NV;
		extensions->vkGetAccelerationStructureMemoryRequirementsNV(
			device.Get(), &ASMemoryRequirementsInfoNV, &memoryRequirements);

		const auto buildRequirements = memoryRequirements.memoryRequirements;

		ASMemoryRequirementsInfoNV.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_UPDATE_SCRATCH_NV;
		extensions->vkGetAccelerationStructureMemoryRequirementsNV(
			device.Get(), &ASMemoryRequirementsInfoNV, &memoryRequirements);

		const auto updateRequirements = memoryRequirements.memoryRequirements;

		return { resultRequirements, buildRequirements, updateRequirements };
	}

	void AccelerationStructure::MemoryBarrier(VkCommandBuffer commandBuffer)
	{
		// Wait for the builder to complete by setting a barrier on the resulting buffer. This is
		// particularly important as the construction of the top-level hierarchy may be called right
		// afterwards, before executing the command list.
		VkMemoryBarrier memoryBarrier = {};
		memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		memoryBarrier.pNext = nullptr;
		memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV |
			VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV;
		memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV |
			VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV;

		vkCmdPipelineBarrier(
			commandBuffer,
			VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV,
			VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV,
			0, 1, &memoryBarrier, 0, nullptr, 0, nullptr);
	}

	ASMemoryRequirementsNV AccelerationStructure::GetTotalRequirements(
		const std::vector<ASMemoryRequirementsNV>& requirements)
	{
		ASMemoryRequirementsNV total{};

		for (const auto& req : requirements)
		{
			total.result.size += req.result.size;
			total.build.size += req.build.size;
			total.update.size += req.update.size;
		}

		return total;
	}
}
