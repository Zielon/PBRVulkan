#pragma once

#include "AccelerationStructure.h"

#include <vector>

namespace Tracer
{
	class Scene;
}

namespace Vulkan
{
	/**
	 * Bottom Level Acceleration Structure
	 */
	class BLAS final : public AccelerationStructure
	{
	public:
		void Generate(
			VkCommandBuffer commandBuffer,
			const class Buffer& scratchBuffer,
			VkDeviceSize scratchOffset,
			const class Buffer& blasBuffer,
			VkDeviceSize resultOffset);

		static VkAccelerationStructureGeometryKHR CreateGeometry(
			const Tracer::Scene& scene,
			uint32_t vertexOffset,
			uint32_t vertexCount,
			uint32_t indexOffset,
			uint32_t indexCount,
			bool isOpaque);

	private:
		std::vector<VkAccelerationStructureGeometryKHR> geometries;
		std::vector<VkAccelerationStructureBuildRangeInfoKHR> buildOffsets;
	};
}
