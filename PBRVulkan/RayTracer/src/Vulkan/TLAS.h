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
		[[nodiscard]] const std::vector<VkGeometryInstance>& GeometryInstances() const
		{
			return geometryInstances;
		}

		void Generate(
			VkCommandBuffer commandBuffer,
			class Buffer& topScratchBuffer,
			VkDeviceSize scratchOffset,
			class Buffer& topBuffer,
			VkDeviceSize topOffset,
			class Buffer& instanceBuffer,
			VkDeviceSize instanceOffset) const;

		static VkGeometryInstance CreateGeometryInstance(
			const class BLAS& blas,
			const glm::mat4& transform,
			uint32_t instanceId);

	private:
		std::vector<VkGeometryInstance> geometryInstances;
	};
}
