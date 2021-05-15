#pragma once

#include "AccelerationStructure.h"

#include <glm/glm.hpp>
#include <vector>

namespace Vulkan
{
	/**
	 * Top Level Acceleration Structure
	 */
	class TLAS final : public AccelerationStructure
	{
	public:
		TLAS(const TLAS&) = delete;
		TLAS& operator = (const TLAS&) = delete;
		TLAS& operator = (TLAS&&) = delete;
		TLAS(TLAS&& other) noexcept;
		TLAS(const class Device& device, VkDeviceAddress instanceAddress, uint32_t instancesCount);
		
		void Generate(
			VkCommandBuffer commandBuffer,
			class Buffer& topScratchBuffer,
			VkDeviceSize scratchOffset,
			class Buffer& topBuffer,
			VkDeviceSize topOffset);

		static VkAccelerationStructureInstanceKHR CreateInstance(
			const class BLAS& blas,
			const glm::mat4& transform,
			uint32_t instanceId);

	private:
		uint32_t instancesCount;
		VkAccelerationStructureGeometryInstancesDataKHR instances{};
		VkAccelerationStructureGeometryKHR geometry{};
	};
}
